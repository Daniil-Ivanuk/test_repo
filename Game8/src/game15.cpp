#include <game15.h>

const State GOAL = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};

bool is_goal(const State &state) { return state == GOAL; }

int find_empty(const State &state) {
    for (int i = 0; i < 16; ++i)
        if (state[i] == 0) return i;
    return -1;
}

std::vector<int> get_possible_moves(int empty_index) {
    std::vector<int> moves{};
    moves.reserve(4);
    int row = empty_index / 4;
    int col = empty_index % 4;

    if (row > 0) moves.push_back(empty_index - 4);
    if (row < 3) moves.push_back(empty_index + 4);
    if (col > 0) moves.push_back(empty_index - 1);
    if (col < 3) moves.push_back(empty_index + 1);
    return moves;
}

State make_move(const State &state, int empty_index, int target_index) {
    State new_state = state;
    std::swap(new_state[empty_index], new_state[target_index]);
    return new_state;
}

int misplaced_tiles(const State& state) {
    int count = 0;
    for (int i = 0; i < 16; ++i)
        if (state[i] != 0 && state[i] != GOAL[i])
            ++count;
    return count;
}

int manhattan_distance(const State& state) {
    int distance = 0;
    for (int i = 0; i < 16; ++i)
        distance += std::abs(state[i] - GOAL[i]);
    return distance;
}