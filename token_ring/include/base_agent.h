#pragma once

#include "data.h"

class BaseAgent {
public:
    BaseAgent(unsigned id, Token&);
    virtual ~BaseAgent() = default;

    void SendMessage(int reciever_id, std::string&& data);
    unsigned Id() const;
    bool HasDataToSend() const;

    virtual Data& Process(Data& recv_data) = 0;
};
