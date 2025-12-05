#include "basic_agent_tests.h"
#include "echo_agent.h"
#include "simple_agent.h"
#include "main_agent.h"
#include "emulator_stub.h"

#include <string>
#include <typeinfo>

namespace {
Token GLOBAL_TOKEN;
const unsigned ID = 1;

void TestCreation() {
    MainAgent agent(ID, GLOBAL_TOKEN);
    BasicTests::TestCreation(agent, ID);
}

void TestSendShortMessage() {
    MainAgent agent(ID, GLOBAL_TOKEN);
    BasicTests::TestSendShortMessage(agent, ID);
}

void TestSendLongMessage() {
    MainAgent agent(ID, GLOBAL_TOKEN);
    BasicTests::TestSendLongMessage(agent, ID);
}

void TestProcess() {
    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"run"});
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);
    BasicTests::TestProcess(*agent, ID, GLOBAL_TOKEN);
    TEST_ASSERT_EQUAL_STR(out.GetLines()[0], "> ");
}

void TestHasActiveAgents() {
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);

    EchoAgent *echo = new EchoAgent(1000, GLOBAL_TOKEN);
    EMULATOR.OnAddAgent(echo);

    BasicTests::TestProcessWithReciever(*agent, *echo, ID, 1000, GLOBAL_TOKEN);
    Data *data = &agent->Process(GLOBAL_TOKEN);
    BasicTests::TestToken(data, GLOBAL_TOKEN);
}

void TestExit() {
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);

    SimpleAgent *simple = new SimpleAgent(1000, GLOBAL_TOKEN);
    EMULATOR.OnAddAgent(simple);

    BasicTests::TestProcessWithReciever(*agent, *simple, ID, 1000, GLOBAL_TOKEN);

    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"", "exit"});

    Data *data = &agent->Process(GLOBAL_TOKEN);
    BasicTests::TestToken(data, GLOBAL_TOKEN);
    TEST_ASSERT_EQUAL_STR(out.GetLines()[0], "> ");
}

void TestAddSimpleAgent() {
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);

    Data *data = &GLOBAL_TOKEN;
    {
        CaptureStream in(std::cin), _(std::cout);
        in.WriteLines({"add 42", "run"});
        data = &agent->Process(GLOBAL_TOKEN);
        BasicTests::TestToken(data, GLOBAL_TOKEN);
        TEST_ASSERT_EQUAL(EMULATOR.agents.size(), 2);
        auto &a = *EMULATOR.agents[42];
        TEST_ASSERT_EQUAL(typeid(a).hash_code(), typeid(SimpleAgent).hash_code());
    }
    {
        CaptureStream in(std::cin), _(std::cout);
        in.WriteLines({"s 1 42 Hi", "run"});
        data = &agent->Process(GLOBAL_TOKEN);
        BasicTests::TestToken(data, GLOBAL_TOKEN);
        {
            CaptureStream out(std::cout);
            TEST_ASSERT_EQUAL(EMULATOR.agents[ID]->HasDataToSend(), true);
            data = &agent->Process(GLOBAL_TOKEN);
            data = &(EMULATOR.agents[42]->Process(*data));
            TEST_ASSERT_EQUAL_STR(out.GetLines()[0], "#42 recv from #1: Hi");
        }
    }
}

void TestAddEchoAgent() {
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);

    Data *data = &GLOBAL_TOKEN;
    {
        CaptureStream in(std::cin), _(std::cout);
        in.WriteLines({"aadd 42", "run"});
        data = &agent->Process(GLOBAL_TOKEN);
        BasicTests::TestToken(data, GLOBAL_TOKEN);
        TEST_ASSERT_EQUAL(EMULATOR.agents.size(), 2);
        auto &a = *EMULATOR.agents[42];
        TEST_ASSERT_EQUAL(typeid(a).hash_code(), typeid(EchoAgent).hash_code());
    }
    {
        CaptureStream in(std::cin), _(std::cout);
        in.WriteLines({"s 1 42 Hi", "run"});
        data = &agent->Process(GLOBAL_TOKEN);
        BasicTests::TestToken(data, GLOBAL_TOKEN);
        {
            CaptureStream out(std::cout);
            TEST_ASSERT_EQUAL(EMULATOR.agents[ID]->HasDataToSend(), true);
            data = &agent->Process(GLOBAL_TOKEN);
            data = &(EMULATOR.agents[42]->Process(*data));
            TEST_ASSERT_EQUAL_STR(out.GetLines()[0], "#42 recv from #1: Hi");
        }
    }
}

void TestDeleteAgent() {
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);

    Data *data = &GLOBAL_TOKEN;
    {
        CaptureStream in(std::cin), _(std::cout);
        in.WriteLines({"aadd 42", "del 42", "run"});
        data = &agent->Process(GLOBAL_TOKEN);
        BasicTests::TestToken(data, GLOBAL_TOKEN);
        TEST_ASSERT_EQUAL(EMULATOR.agents.size(), 1);
    }
}

void TestSequence() {
    TEST_EMULATOR EMULATOR;
    MainAgent *agent = new MainAgent(ID, GLOBAL_TOKEN);
    agent->AddListener(&EMULATOR);

    Data *data = &GLOBAL_TOKEN;
    {
        CaptureStream in(std::cin), _(std::cout);
        in.WriteLines({"aadd 42", "del 42", "add 42", "s 42 -1 Hi", "exit"});
        data = &agent->Process(GLOBAL_TOKEN);
        BasicTests::TestToken(data, GLOBAL_TOKEN);
        TEST_ASSERT_EQUAL(EMULATOR.agents.size(), 2);
        TEST_ASSERT_EQUAL(EMULATOR.agents[42]->HasDataToSend(), true);
        auto &a = *EMULATOR.agents[42];
        TEST_ASSERT_EQUAL(typeid(a).hash_code(), typeid(SimpleAgent).hash_code());
    }
}

}  // namespace

namespace test_main_agent {
void RunTests() {
    TestRunner runner;
    runner.RunTest("Creation", TestCreation);
    runner.RunTest("Send Short Message", TestSendShortMessage);
    runner.RunTest("Send Long Message", TestSendLongMessage);
    runner.RunTest("Process", TestProcess);
    runner.RunTest("Has Active Agents", TestHasActiveAgents);
    runner.RunTest("Exit", TestExit);
    runner.RunTest("Add Simple Agents", TestAddSimpleAgent);
    runner.RunTest("Add Echo Agents", TestAddEchoAgent);
    runner.RunTest("Delete Agent", TestDeleteAgent);
    runner.RunTest("Sequence", TestSequence);
}
}  // namespace test_main_agent
