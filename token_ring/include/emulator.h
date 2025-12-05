#pragma once

#include "data.h"
#include "main_agent.h"

class BaseAgent;

class Emulator : public IListener {
public:
    bool OnAddAgent(BaseAgent* agent) override;
    bool OnDeleteAgent(unsigned agent_id) override;
    void Run();

    bool HasActiveAgents() const override;
    bool OnSendMessage(unsigned from, unsigned to, std::string&& msg) override;
    void OnExit() override;

    ~Emulator();
};
