#include "simple_agent.h"
#include "basic_agent_tests.h"

#include <string>
#include <typeinfo>

namespace {
Token GLOBAL_TOKEN;

void TestCreation() {
    SimpleAgent agent(1, GLOBAL_TOKEN);
    BasicTests::TestCreation(agent, 1);
}

void TestSendShortMessage() {
    SimpleAgent agent(2, GLOBAL_TOKEN);
    BasicTests::TestSendShortMessage(agent, 2);
}

void TestSendLongMessage() {
    SimpleAgent agent(3, GLOBAL_TOKEN);
    BasicTests::TestSendLongMessage(agent, 3);
}

void TestProcess() {
    SimpleAgent agent(4, GLOBAL_TOKEN);
    BasicTests::TestProcess(agent, 4, GLOBAL_TOKEN);
}

void TestProcessWithReciever() {
    SimpleAgent sender(5, GLOBAL_TOKEN), reciever(6, GLOBAL_TOKEN);
    BasicTests::TestProcessWithReciever(sender, reciever, 5, 6, GLOBAL_TOKEN);
}

void TestProcessLongMessage() {
    SimpleAgent sender(7, GLOBAL_TOKEN), reciever(8, GLOBAL_TOKEN);
    BasicTests::TestProcessLongMessage(sender, reciever, 7, 8, GLOBAL_TOKEN);
}

void TestMultipleSenders() {
    SimpleAgent sender_1(1, GLOBAL_TOKEN), sender_2(2, GLOBAL_TOKEN);
    SimpleAgent reciever(3, GLOBAL_TOKEN);
    
    CaptureStream out(std::cout);
    sender_1.SendMessage(3, "hello from sender 1! how are you?");
    sender_2.SendMessage(3, "hello from sender 2! how are you?");

    Data *data = &GLOBAL_TOKEN;

    while (sender_1.HasDataToSend() || sender_2.HasDataToSend()) {
        data = &sender_1.Process(*data);
        data = &reciever.Process(*data);
        data = &sender_2.Process(*data);
    }
    
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 2);
    TEST_ASSERT_EQUAL_STR(lines[0], "#3 recv from #1: hello from sender 1! how are you?");
    TEST_ASSERT_EQUAL_STR(lines[1], "#3 recv from #2: hello from sender 2! how are you?");
}

void TestBroadcast() {
    SimpleAgent sender(1, GLOBAL_TOKEN);
    SimpleAgent reciever_2(2, GLOBAL_TOKEN), reciever_3(3, GLOBAL_TOKEN);
    
    CaptureStream out(std::cout);
    sender.SendMessage(-1, "hello everybody!");

    Data *data = &GLOBAL_TOKEN;

    while (sender.HasDataToSend()) {
        data = &sender.Process(*data);
        data = &reciever_2.Process(*data);
        data = &reciever_3.Process(*data);
    }
    
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 2);
    TEST_ASSERT_EQUAL_STR(lines[0], "#2 recv from #1: hello everybody!");
    TEST_ASSERT_EQUAL_STR(lines[1], "#3 recv from #1: hello everybody!");
}
}  // namespace

namespace test_simple_agent {
void RunTests() {
    TestRunner runner;
    runner.RunTest("Creation", TestCreation);
    runner.RunTest("Send Short Message", TestSendShortMessage);
    runner.RunTest("Send Long Message", TestSendLongMessage);
    runner.RunTest("Process", TestProcess);
    runner.RunTest("Process With Reciever", TestProcessWithReciever);
    runner.RunTest("Process Long Message", TestProcessLongMessage);
    runner.RunTest("Multiple Senders", TestMultipleSenders);
    runner.RunTest("Broadcast", TestBroadcast);
}
}  // namespace test_simple_agent
