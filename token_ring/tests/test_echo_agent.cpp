#include "basic_agent_tests.h"
#include "echo_agent.h"

#include <string>
#include <typeinfo>

namespace {
Token GLOBAL_TOKEN;

void TestCreation() {
    EchoAgent agent(1, GLOBAL_TOKEN);
    BasicTests::TestCreation(agent, 1);
}

void TestSendShortMessage() {
    EchoAgent agent(2, GLOBAL_TOKEN);
    BasicTests::TestSendShortMessage(agent, 2);
}

void TestSendLongMessage() {
    EchoAgent agent(3, GLOBAL_TOKEN);
    BasicTests::TestSendLongMessage(agent, 3);
}

void TestProcess() {
    EchoAgent agent(4, GLOBAL_TOKEN);
    BasicTests::TestProcess(agent, 4, GLOBAL_TOKEN);
}

void TestProcessWithReciever() {
    EchoAgent sender(5, GLOBAL_TOKEN), reciever(6, GLOBAL_TOKEN);
    BasicTests::TestProcessWithReciever(sender, reciever, 5, 6, GLOBAL_TOKEN);
}

void TestProcessLongMessage() {
    EchoAgent sender(7, GLOBAL_TOKEN), reciever(8, GLOBAL_TOKEN);
    BasicTests::TestProcessLongMessage(sender, reciever, 7, 8, GLOBAL_TOKEN);
}

void TestEchoMessage() {
    EchoAgent sender(5, GLOBAL_TOKEN), reciever(6, GLOBAL_TOKEN);
    BasicTests::TestProcessWithReciever(sender, reciever, 5, 6, GLOBAL_TOKEN);

    {
        CaptureStream out(std::cout);

        TEST_ASSERT_EQUAL(reciever.HasDataToSend(), true);
        Data *data = &reciever.Process(GLOBAL_TOKEN);
        data = &sender.Process(*data);
        TEST_ASSERT_EQUAL_STR(out.GetLines()[0], "#5 recv from #6: hi");
    }
}

void TestEchoLongMessage() {
    EchoAgent sender(7, GLOBAL_TOKEN), reciever(8, GLOBAL_TOKEN);
    BasicTests::TestProcessLongMessage(sender, reciever, 7, 8, GLOBAL_TOKEN);

    {
        CaptureStream out(std::cout);

        TEST_ASSERT_EQUAL(reciever.HasDataToSend(), true);
        Data *data = &GLOBAL_TOKEN;
        while (reciever.HasDataToSend()) {
            data = &reciever.Process(*data);
            data = &sender.Process(*data);
        }
        TEST_ASSERT_EQUAL_STR(out.GetLines()[0],
                              "#7 recv from #8: me??? hear you do heeey!!!! here? anybody is hi!");
    }
}
}  // namespace

namespace test_echo_agent {
void RunTests() {
    TestRunner runner;
    runner.RunTest("Creation", TestCreation);
    runner.RunTest("Send Short Message", TestSendShortMessage);
    runner.RunTest("Send Long Message", TestSendLongMessage);
    runner.RunTest("Process", TestProcess);
    runner.RunTest("Process With Reciever", TestProcessWithReciever);
    runner.RunTest("Process Long Message", TestProcessLongMessage);
    runner.RunTest("Echo Message", TestEchoMessage);
    runner.RunTest("Echo Long Message", TestEchoLongMessage);
}
}  // namespace test_echo_agent
