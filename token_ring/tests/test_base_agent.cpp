#include "base_agent.h"
#include "basic_agent_tests.h"
#include <string>

namespace {
Token GLOBAL_TOKEN;

class TestAgent : public BaseAgent {
public:
    std::vector<std::string> received_messages;
    size_t send_msg_counter{0};
    size_t send_token_counter{0};

    TestAgent(int id) : BaseAgent(id, GLOBAL_TOKEN) {
    }

    Data& Process(Data& recv_data) override {
        return recv_data;
    }
};

void TestCreation() {
    TestAgent agent(1);
    BasicTests::TestCreation(agent, 1);
}

void TestSendShortMessage() {
    TestAgent agent(2);
    BasicTests::TestSendShortMessage(agent, 2);
}

void TestSendLongMessage() {
    TestAgent agent(3);
    BasicTests::TestSendLongMessage(agent, 3);
}
}  // namespace

namespace test_base_agent {
Token GLOBAL_TOKEN;

void RunTests() {
    TestRunner runner;
    runner.RunTest("Creation", TestCreation);
    runner.RunTest("Send Short Message", TestSendShortMessage);
    runner.RunTest("Send Long Message", TestSendLongMessage);
}
}  // namespace test_base_agent
