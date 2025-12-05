namespace test_data{
void RunTests();
}

namespace test_base_agent {
void RunTests();
}

namespace test_simple_agent {
void RunTests();
}

namespace test_echo_agent{
void RunTests();
}

namespace test_main_agent{
void RunTests();
}

namespace test_emulator{
void RunTests();
}

int main() {
    test_data::RunTests();
    test_base_agent::RunTests();
    test_simple_agent::RunTests();
    test_echo_agent::RunTests();
    test_main_agent::RunTests();
    test_emulator::RunTests();
}
