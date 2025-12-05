#pragma once

#include "base_agent.h"

class EchoAgent : public BaseAgent {
public:
    EchoAgent(unsigned id, Token& token);
    Data& Process(Data& recv_data) override;
};
