#include "data.h"
#include "common.h"

#include <string>
#include <cstring>

namespace {

void TestTokenCreation() {
    Token token;
}

void TestMessageCreation() {
    Message msg(
        1 /* sender */,
        2 /* reciever */,
        "hello" /* message */,
        1 /* chunk id */,
        10 /* chunk number */
    );
    auto header = msg.header;
    auto chunk = msg.chunk;

    TEST_ASSERT_EQUAL(header.sender_id, 1);
    TEST_ASSERT_EQUAL(header.reciever_id, 2);

    TEST_ASSERT_EQUAL(chunk.id, 1);
    TEST_ASSERT_EQUAL(chunk.total, 10);
    TEST_ASSERT_EQUAL(chunk.size, std::strlen("hello"));

    TEST_ASSERT_EQUAL(strcmp(msg.data, "hello"), 0);
}
}  // namespace

namespace test_data {

void RunTests() {
    TestRunner runner;
    runner.RunTest("Token Creation", TestTokenCreation);
    runner.RunTest("Message Creation", TestMessageCreation);
}
}  // namespace test_data
