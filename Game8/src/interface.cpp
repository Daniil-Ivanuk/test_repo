#include <interface.h>

void print_state(const State &state) {
    for (int i = 0; i < 16; ++i) {
        std::cout << state[i] << " ";
        if ((i + 1) % 4 == 0) std::cout << std::endl;
    }
    std::cout << std::endl; return;
}

State parse_state(const std::string &input) {
    State state;
    state.reserve(16);
    std::stringstream ss(input);
    int num;
    while (ss >> num) state.push_back(num);
    return state;
}
