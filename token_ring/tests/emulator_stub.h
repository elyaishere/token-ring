#pragma once

#include "data.h"
#include "base_agent.h"
#include "main_agent.h"

#include <unordered_map>

class TEST_EMULATOR : public IListener {
public:
    bool OnAddAgent(BaseAgent* agent) override {
        agents[agent->Id()] = agent;
        return true;
    }

    bool OnDeleteAgent(unsigned agent_id) override {
        delete agents[agent_id];
        agents.erase(agent_id);
        return true;
    }

    bool HasActiveAgents() const override {
        for (auto [_, agent] : agents) {
            if (agent->HasDataToSend()) {
                return true;
            }
        }
        return false;
    }

    bool OnSendMessage(unsigned from, unsigned to, std::string&& msg) override {
        agents[from]->SendMessage(to, std::move(msg));
        return true;
    }

    void OnExit() override { /* nop */
    }

    ~TEST_EMULATOR() {
        for (auto [id, agent]: agents) {
            delete agent;
        }
    }

    std::unordered_map<unsigned, BaseAgent*> agents;
};
