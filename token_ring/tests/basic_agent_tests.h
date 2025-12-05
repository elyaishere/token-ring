#pragma once

#include "common.h"
#include "base_agent.h"
#include "data.h"

struct BasicTests {

    static void TestCreation(BaseAgent &agent, unsigned id) {
        TEST_ASSERT_EQUAL(agent.Id(), id);
        TEST_ASSERT_EQUAL(agent.HasDataToSend(), false);
    }

    static void TestSendShortMessage(BaseAgent &agent, unsigned id) {
        TEST_ASSERT_EQUAL(agent.Id(), id);
        TEST_ASSERT_EQUAL(agent.HasDataToSend(), false);
        agent.SendMessage(-1, "a");
        TEST_ASSERT_EQUAL(agent.HasDataToSend(), true);
    }

    static void TestSendLongMessage(BaseAgent &agent, unsigned id) {
        TEST_ASSERT_EQUAL(agent.Id(), id);
        TEST_ASSERT_EQUAL(agent.HasDataToSend(), false);
        agent.SendMessage(-1, std::string(100, 'a'));
        TEST_ASSERT_EQUAL(agent.HasDataToSend(), true);
    }

    static void TestToken(Data *data, Token &token) {
        TEST_ASSERT(typeid(*data) == typeid(Token), "should pass token back");
        TEST_ASSERT(data == std::addressof(token), "should pass *the same* token back");
    }

    static void TestProcess(BaseAgent &agent, unsigned id, Token &token) {
        TestCreation(agent, id);

        Data *data = &agent.Process(token);
        TestToken(data, token);

        agent.SendMessage(-1, std::string(100, 'a'));
        TEST_ASSERT_EQUAL(agent.HasDataToSend(), true);

        while (agent.HasDataToSend()) {
            data = &agent.Process(*data);
            TEST_ASSERT(typeid(*data) == typeid(Message), "should pass message chunk");

            Message &msg = dynamic_cast<Message &>(*data);
            auto [sender_id, reciever_id] = msg.header;
            TEST_ASSERT_EQUAL((unsigned)sender_id, id);
            TEST_ASSERT_EQUAL(reciever_id, -1);

            data = &agent.Process(*data);
            TestToken(data, token);
        }
    }

    static void TestProcessWithReciever(BaseAgent &sender, BaseAgent &reciever, unsigned id_sender,
                                        int id_reciever, Token &token) {
        CaptureStream out{std::cout};

        sender.SendMessage(id_reciever, "hi");

        Data *data = &reciever.Process(token);
        TestToken(data, token);

        data = &sender.Process(*data);
        TEST_ASSERT(typeid(*data) == typeid(Message), "send message");
        Message &msg = dynamic_cast<Message &>(*data);
        auto [sender_id, reciever_id] = msg.header;
        TEST_ASSERT_EQUAL((unsigned)sender_id, id_sender);
        TEST_ASSERT_EQUAL(reciever_id, id_reciever);

        data = &reciever.Process(*data);
        TEST_ASSERT(typeid(*data) == typeid(Message), "send message back");
        TEST_ASSERT_EQUAL_STR(out.GetLines()[0], "#" + std::to_string(id_reciever) +
                                                     " recv from #" + std::to_string(id_sender) +
                                                     ": hi");

        data = &sender.Process(*data);
        TestToken(data, token);
        TEST_ASSERT_EQUAL(sender.HasDataToSend(), false);
    }

    static void TestProcessLongMessage(BaseAgent &sender, BaseAgent &reciever, unsigned id_sender,
                                       unsigned id_reciever, Token &token) {
        CaptureStream out(std::cout);

        sender.SendMessage(id_reciever, "hi! is anybody here? heeey!!!! do you hear me???");
        TEST_ASSERT_EQUAL(sender.HasDataToSend(), true);

        Data *data = &sender.Process(token);
        TEST_ASSERT(typeid(*data) == typeid(Message), "send message");

        bool sender_turn = true;
        unsigned loops = 0;

        while (sender.HasDataToSend()) {
            data = &reciever.Process(*data);
            if (sender_turn) {
                TEST_ASSERT(typeid(*data) == typeid(Message), "send message back");
            } else {
                TestToken(data, token);
            }

            data = &sender.Process(*data);

            sender_turn = !sender_turn;
            ++loops;
        }

        TEST_ASSERT_EQUAL_STR(out.GetLines()[0],
                              "#" + std::to_string(id_reciever) + " recv from #" +
                                  std::to_string(id_sender) +
                                  ": hi! is anybody here? heeey!!!! do you hear me???");
        TEST_ASSERT(loops > 1, "this is a long message");
    }
};
