#pragma once

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <iomanip>
#include <vector>


struct CaptureStream {
    CaptureStream(std::ios &io) : old(io.rdbuf(local.rdbuf())), io(io) {
    }
    ~CaptureStream() {
        io.rdbuf(old);
    }

    void WriteLines(const std::vector<std::string>& lines) {
        for (const auto & line: lines) {
            local << line << std::endl;
        }
    }

    std::vector<std::string> GetLines() {
        std::vector<std::string> lines;
        std::string line;
        char buffer[3]{'\0'};
        while (std::getline(local, line)) {
            std::istringstream ss(std::move(line));
            while (ss.peek() == '>') {
                ss.read(buffer, 2);
                lines.push_back(buffer);
            }
            if (ss.peek() != EOF) {
                std::getline(ss, line);
                lines.push_back(std::move(line));
            }
        }
        return lines;
    }

    std::stringstream local;
    std::streambuf *old;
    std::ios &io;
};

class TestFailure : public std::exception {
public:
    TestFailure(const std::string& message, const char* function, int line)
        : message_(BuildMessage(message, function, line)) {
    }

    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    static std::string BuildMessage(const std::string& message, const char* function, int line) {
        std::ostringstream oss;
        oss << "Failed test " << function << " at line " << line << ": " << message;
        return oss.str();
    }

    std::string message_;
};

class TestRunner {
public:
    ~TestRunner() {
        if (failed_count_ > 0) {
            std::cerr << "\n=== TEST SUMMARY ===\n";
            std::cerr << "Failed: " << failed_count_ << "/" << total_count_ << " tests\n";
            assert(0);
        } else {
            std::cerr << "\n=== ALL TESTS PASSED ===\n\n";
        }
    }

    template <typename TestFunc>
    void RunTest(const std::string& test_name, TestFunc test_func) {
        std::cerr << "Running " << test_name << "... ";
        ++total_count_;

        try {
            test_func();
            std::cerr << std::setw(50 - test_name.size()) << "PASS\n";
        } catch (const std::exception& e) {
            ++failed_count_;
            std::cerr << std::setw(50 - test_name.size()) << "FAIL\n" << e.what() << "\n";
        } catch (...) {
            ++failed_count_;
            std::cerr << std::setw(50 - test_name.size()) << "FAIL\n" << "Unknown exception\n";
        }
    }

private:
    size_t total_count_ = 0;
    size_t failed_count_ = 0;
};

#define TEST_ASSERT(condition, message)                              \
    if (!(condition)) {                                              \
        throw TestFailure((message), __PRETTY_FUNCTION__, __LINE__); \
    }

#define TEST_ASSERT_EQUAL_STR(expected, actual) \
    TEST_ASSERT((expected) == (actual),         \
                "Expected " #expected " == " #actual " (" + (expected) + " == " + (actual) + ")")

#define TEST_ASSERT_EQUAL(expected, actual)                                         \
    TEST_ASSERT((expected) == (actual), "Expected " #expected " == " #actual " (" + \
                                            std::to_string(expected) +              \
                                            " == " + std::to_string(actual) + ")")
