#include <common.h>

bool is_goal(const State& state);

int find_empty(const State& state);

std::vector<int> get_possible_moves(int empty_index);

int misplaced_tiles(const State& state);
int manhattan_distance(const State& state);

State make_move(const State& state, int empty_index, int target_index);

