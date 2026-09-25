#include <ai.h>
#include <interface.h>

int main() {
    std::string input = "3 5 8 0 6 4 1 7 2 9 15 11 13 12 14 10";
    State start = parse_state(input);
    auto solution = BFS_solve(start);
    print_state(start);
    if (solution.first.empty()) std::cout << "Solution was not found" << std::endl;
    else for (auto state : solution.first) print_state(state);
    std::cout << "Solution has been found by " << solution.first.size() << " turns" << std::endl;
    std::cout << "Overall states has been visited " << solution.second << " states" << std::endl;
    std::cout << "P = " << solution.first.size() / static_cast<double>(solution.second);
    
    return 0;
}

