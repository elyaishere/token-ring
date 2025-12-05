#pragma once

#include "base_agent.h"

class SimpleAgent : public BaseAgent {
public:
    SimpleAgent(unsigned id, Token& token);
    Data& Process(Data& recv_data) override;
};
