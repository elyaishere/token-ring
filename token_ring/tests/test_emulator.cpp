#include "common.h"
#include "echo_agent.h"
#include "simple_agent.h"
#include "main_agent.h"
#include "emulator.h"

#include <set>
#include <unordered_map>

namespace {
Token GLOBAL_TOKEN;

void TestCreation() {
    Emulator emulator;
    TEST_ASSERT_EQUAL(emulator.HasActiveAgents(), false);
}

void TestAddDeleteAgents() {
    Emulator emulator;
    TEST_ASSERT_EQUAL(emulator.HasActiveAgents(), false);

    SimpleAgent* agent_1 = new SimpleAgent(1, GLOBAL_TOKEN);
    SimpleAgent* agent_2 = new SimpleAgent(2, GLOBAL_TOKEN);
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(agent_1), true);
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(agent_2), true);
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(agent_1), false);

    TEST_ASSERT_EQUAL(emulator.OnDeleteAgent(1), true);
    TEST_ASSERT_EQUAL(emulator.OnDeleteAgent(3), false);
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(new SimpleAgent(1, GLOBAL_TOKEN)), true);
}

void TestSendMessage() {
    Emulator emulator;
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(new SimpleAgent(1, GLOBAL_TOKEN)), true);
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(new SimpleAgent(2, GLOBAL_TOKEN)), true);
    TEST_ASSERT_EQUAL(emulator.OnAddAgent(new EchoAgent(3, GLOBAL_TOKEN)), true);
    TEST_ASSERT_EQUAL(emulator.HasActiveAgents(), false);
    TEST_ASSERT_EQUAL(emulator.OnSendMessage(3, 1, "hello"), false);
    TEST_ASSERT_EQUAL(emulator.HasActiveAgents(), false);

    TEST_ASSERT_EQUAL(emulator.OnSendMessage(1, 2, "hello"), true);
    TEST_ASSERT_EQUAL(emulator.HasActiveAgents(), true);
}

class TestAgent : public BaseAgent {
public:
    static std::unordered_map<unsigned, size_t> token_passed;
    static std::unordered_map<unsigned, std::pair<size_t, size_t>> messages_recieved;

    TestAgent(unsigned id, Token& token) : BaseAgent(id, token) {
    }

    Data& Process(Data& recv_data) override {
        if (recv_data.data_type == DataType::kToken) {
            ++token_passed[Id()];
        } else {  // message
            ++messages_recieved[Id()].second;
            if (auto reciever_id = dynamic_cast<Message&>(recv_data).header.reciever_id;
                reciever_id == (int)Id() || reciever_id == -1) {
                ++messages_recieved[Id()].first;
            }
        }
        return recv_data;
    }
};

std::unordered_map<unsigned, size_t> TestAgent::token_passed;
std::unordered_map<unsigned, std::pair<size_t, size_t>> TestAgent::messages_recieved;

void TestTokenCirculation() {
    TestAgent::token_passed.clear();
    TestAgent::messages_recieved.clear();

    Emulator emulator;
    emulator.OnAddAgent(new TestAgent(1, GLOBAL_TOKEN /*fake*/));
    emulator.OnAddAgent(new TestAgent(2, GLOBAL_TOKEN /*fake*/));
    CaptureStream in(std::cin), _(std::cout);
    in.WriteLines({"run", "exit"});
    emulator.Run();
    TEST_ASSERT_EQUAL(TestAgent::token_passed[1], 1);
    TEST_ASSERT_EQUAL(TestAgent::token_passed[2], 1);
}

void TestMessageCirculation() {
    TestAgent::token_passed.clear();
    TestAgent::messages_recieved.clear();

    Emulator emulator;
    emulator.OnAddAgent(new TestAgent(1, GLOBAL_TOKEN /*fake*/));
    emulator.OnAddAgent(new TestAgent(2, GLOBAL_TOKEN /*fake*/));
    CaptureStream in(std::cin), _(std::cout);
    in.WriteLines({"s 0 1 hello", "run", "exit"});
    emulator.Run();
    TEST_ASSERT_EQUAL(TestAgent::token_passed[1], 2);
    TEST_ASSERT_EQUAL(TestAgent::token_passed[2], 2);
    TEST_ASSERT_EQUAL(TestAgent::messages_recieved[1].first, 1);
    TEST_ASSERT_EQUAL(TestAgent::messages_recieved[1].second, 1);
    TEST_ASSERT_EQUAL(TestAgent::messages_recieved[2].first, 0);
    TEST_ASSERT_EQUAL(TestAgent::messages_recieved[2].second, 1);
}

void TestBasicCommunication() {
    Emulator emulator;
    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"add 1", "aadd 2", "s 1 2 Hello Echo", "run", "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 7);
    TEST_ASSERT_EQUAL_STR(lines[0], "> ");
    TEST_ASSERT_EQUAL_STR(lines[4], "#2 recv from #1: Hello Echo");
    TEST_ASSERT_EQUAL_STR(lines[5], "#1 recv from #2: Echo Hello");
    TEST_ASSERT_EQUAL_STR(lines[6], "> ");
}

void TestBroadcast() {
    Emulator emulator;
    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"add 1", "add 2", "add 3", "s 1 -1 Hello everybody!", "run", "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 9);
    TEST_ASSERT_EQUAL_STR(lines[0], "> ");
    std::set<std::string> messages{lines[5], lines[6], lines[7]};
    TEST_ASSERT_EQUAL_STR(*messages.begin(), "#0 recv from #1: Hello everybody!");
    TEST_ASSERT_EQUAL_STR(*std::next(messages.begin()), "#2 recv from #1: Hello everybody!");
    TEST_ASSERT_EQUAL_STR(*std::prev(messages.end()), "#3 recv from #1: Hello everybody!");
    TEST_ASSERT_EQUAL_STR(lines[8], "> ");
}

void TestLongMessage() {
    Emulator emulator;
    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines(
        {"add 1", "add 2",
         "s 1 2 Hi! How are you doing?? I've been really busy lately, so I have to go, sorry!",
         "run", "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 6);
    TEST_ASSERT_EQUAL_STR(lines[0], "> ");
    TEST_ASSERT_EQUAL_STR(lines[4],
                          "#2 recv from #1: Hi! How are you doing?? I've been really busy lately, "
                          "so I have to go, sorry!");
    TEST_ASSERT_EQUAL_STR(lines[5], "> ");
}

void TestBroadcastLong() {
    TestAgent::token_passed.clear();
    TestAgent::messages_recieved.clear();

    Emulator emulator;
    emulator.OnAddAgent(new TestAgent(42, GLOBAL_TOKEN /*fake*/));
    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines(
        {"add 1",
         "s 1 -1 Hi! How are you doing?? I've been really busy lately, so I have to go, sorry!",
         "run", "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 5);
    TEST_ASSERT_EQUAL_STR(lines[0], "> ");
    TEST_ASSERT_EQUAL_STR(lines[3],
                          "#0 recv from #1: Hi! How are you doing?? I've been really busy lately, "
                          "so I have to go, sorry!");
    TEST_ASSERT_EQUAL_STR(lines[4], "> ");
    TEST_ASSERT(TestAgent::messages_recieved[42].first > 1, "message must be fragmented");
}

void TestManySenders() {
    Emulator emulator;

    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"add 1", "add 2", "add 3", "s 1 2 Hi Agent 2", "s 1 3 Hi Agent 3",
                   "s 2 3 Hi Agent 3", "run", "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 11);
    TEST_ASSERT_EQUAL_STR(lines[0], "> ");
    std::set<std::string> messages{lines[7], lines[8], lines[9]};
    TEST_ASSERT_EQUAL_STR(*messages.begin(), "#2 recv from #1: Hi Agent 2");
    TEST_ASSERT_EQUAL_STR(*std::next(messages.begin()), "#3 recv from #1: Hi Agent 3");
    TEST_ASSERT_EQUAL_STR(*std::prev(messages.end()), "#3 recv from #2: Hi Agent 3");
    TEST_ASSERT_EQUAL_STR(lines[10], "> ");
}

void TestRepeat() {
    Emulator emulator;

    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"add 1", "add 2", "add 3", "s 1 2 Hi Agent 2", "s 1 3 Hi Agent 3",
                   "s 2 3 Hi Agent 3", "run", "s 3 -1 Goodbye everybody!", "run", "del 3", "run",
                   "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 18);
    TEST_ASSERT_EQUAL_STR(lines[0], "> ");
    {
        std::set<std::string> messages{lines[7], lines[8], lines[9]};
        TEST_ASSERT_EQUAL_STR(*messages.begin(), "#2 recv from #1: Hi Agent 2");
        TEST_ASSERT_EQUAL_STR(*std::next(messages.begin()), "#3 recv from #1: Hi Agent 3");
        TEST_ASSERT_EQUAL_STR(*std::prev(messages.end()), "#3 recv from #2: Hi Agent 3");
    }
    TEST_ASSERT_EQUAL_STR(lines[10], "> ");
    {
        std::set<std::string> messages{lines[12], lines[13], lines[14]};
        TEST_ASSERT_EQUAL_STR(*messages.begin(), "#0 recv from #3: Goodbye everybody!");
        TEST_ASSERT_EQUAL_STR(*std::next(messages.begin()), "#1 recv from #3: Goodbye everybody!");
        TEST_ASSERT_EQUAL_STR(*std::prev(messages.end()), "#2 recv from #3: Goodbye everybody!");
    }
    TEST_ASSERT_EQUAL_STR(lines[15], "> ");
    TEST_ASSERT_EQUAL_STR(lines[17], "> ");
}

void TestCorrectness() {
    Emulator emulator;

    CaptureStream in(std::cin), out(std::cout);
    in.WriteLines({"add 1", "add 1", "aadd 1", "aadd 2", "run", "del 1", "del 3", "del 0", "run",
                   "s 1 2 Hello from underground", "s 2 -1 Echo?", "run", "exit"});
    emulator.Run();
    auto lines = out.GetLines();
    TEST_ASSERT_EQUAL(lines.size(), 19);
    TEST_ASSERT(lines[2][0] != '>',
                "should be error message (e.g. `Cannot add Simple Agent with id #1`)");
    TEST_ASSERT(lines[4][0] != '>',
                "should be error message (e.g. `Cannot add Echo Agent with id #1`)");
    TEST_ASSERT(lines[9][0] != '>',
                "should be error message (e.g. `Cannot delete Agent with id #3`)");
    TEST_ASSERT(lines[11][0] != '>',
                "should be error message (e.g. `Cannot delete Agent with id #0`)");
    TEST_ASSERT(lines[14][0] != '>',
                "should be error message (e.g. `Cannot send message from agent #1 to agent #2`)");
    TEST_ASSERT(lines[16][0] != '>',
                "should be error message (e.g. `Cannot send message from agent #2 to everybody");
}
}  // namespace

namespace test_emulator {
void RunTests() {
    TestRunner runner;
    runner.RunTest("Creation", TestCreation);
    runner.RunTest("Add Delete Agents", TestAddDeleteAgents);
    runner.RunTest("Send Message", TestSendMessage);
    runner.RunTest("Token Circulation", TestTokenCirculation);
    runner.RunTest("Message Circulation", TestMessageCirculation);
    runner.RunTest("Basic Communication", TestBasicCommunication);
    runner.RunTest("Broadcast", TestBroadcast);
    runner.RunTest("Long Message", TestLongMessage);
    runner.RunTest("Broadcast Long", TestBroadcastLong);
    runner.RunTest("Many Senders", TestManySenders);
    runner.RunTest("Repeat", TestRepeat);
    runner.RunTest("Correctness", TestCorrectness);
}
}  // namespace test_emulator
