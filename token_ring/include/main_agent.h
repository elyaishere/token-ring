#pragma once

#include "base_agent.h"
#include <string>

class IListener {
public:
    virtual ~IListener() = default;
    virtual bool OnAddAgent(BaseAgent* agent) = 0;
    virtual bool OnDeleteAgent(unsigned id) = 0;
    virtual bool OnSendMessage(unsigned from, int to, std::string&& msg) = 0;
    virtual bool HasActiveAgents() const = 0;
    virtual void OnExit() = 0;
};

class MainAgent : public BaseAgent {
public:
    MainAgent(unsigned id, Token& token);
    Data& Process(Data& recv_data) override;
    void AddListener(IListener*);
};
