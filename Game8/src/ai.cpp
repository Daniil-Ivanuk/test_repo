#include <ai.h>
#include <numeric>
#include <iterator>
#include <game15.h>

const State GOAL = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};

size_t StateHash::operator()(const State &state) const {
    return std::accumulate(std::begin(state), std::end(state), size_t(0), 
        [](size_t h, int tile){
            return h * 17 + tile; 
        });
}

std::vector<State> reconstruct_path(const std::unordered_map<State, State, StateHash> &parent, State current) {
    std::vector<State> path{};
    auto it = parent.find(current);
    while ((!(it == parent.end()) && !(it->second == current))) {
        path.push_back(current);
        current = it->second;
        it = parent.find(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::pair<std::vector<State>, int> BFS_solve(State init_state) {
    if (is_goal(init_state)) return {{init_state}, 0};
    int count_visited = 0;
    std::queue<State> open;
    std::unordered_set<State, StateHash> closed;
    std::unordered_map<State, State, StateHash> parent;

    open.push(init_state);
    closed.insert(init_state);
    parent[init_state] = init_state;

    while(!open.empty()) {
        State current = open.front();
        open.pop();
        ++count_visited;
        int empty_index = find_empty(current);
        std::vector<int> moves = get_possible_moves(empty_index);
        for (auto target_idx : moves) {
            State next_state = make_move(current, empty_index, target_idx);
            if (closed.find(next_state) == closed.end()) {
                closed.insert(next_state);
                parent[next_state] = current;
                open.push(next_state);
                
                if (is_goal(next_state)) 
                    return {reconstruct_path(parent, next_state), count_visited};
            }
        }
    }
    return {std::vector<State>(), count_visited};
}

bool dls(const State &state, int depth, int limit, std::vector<State> &solution, std::unordered_set<State, StateHash> &closed, int& count_visited) {
    if (depth > limit) return false;
    solution.push_back(state);
    closed.insert(state);

    if (is_goal(state)) return true;

    int empty_idx = find_empty(state);
    auto moves = get_possible_moves(empty_idx);
    ++count_visited;
    for (int target_idx : moves) {
        State next_state = make_move(state, empty_idx, target_idx);
        if (closed.find(next_state) == closed.end())
            if (dls(next_state, depth + 1, limit, solution, closed, count_visited)) 
                return true;
    }
    solution.pop_back();
    closed.erase(state);
    return false;
}

std::pair<std::vector<State>, int> DLS_solve(State init_state, int limit) {
    if (is_goal(init_state)) return {{init_state}, 0};
    int count_visited = 0;
    std::vector<State> solution;
    std::unordered_set<State, StateHash> closed;
    
    for (int depth = 0; depth <= limit; ++depth) {
        if (dls(init_state, 0, limit, solution, closed, count_visited)) 
            return {solution, count_visited};
        solution.clear();
        closed.clear();
    }

    return {std::vector<State>(), count_visited};
}

std::pair<std::vector<State>, int> EST_solve(State init_state, int (*est)(const State&)) {
    if (is_goal(init_state)) return {{init_state}, 0};
    
    int count_visited = 0;
    using StateEst = std::pair<State, int>;
    
    auto cmp = [](StateEst& a,  StateEst& b) {
        return a.second > b.second;
    };
    std::priority_queue<StateEst, std::vector<StateEst>, decltype(cmp)> open(cmp);
    std::unordered_set<State, StateHash> closed;
    std::unordered_map<State, State, StateHash> parent;
    std::unordered_map<State, int, StateHash> g_score;

    g_score[init_state] = 0;
    int est_score = g_score[init_state] + est(init_state);
    open.push({init_state, est_score});
    parent[init_state] = init_state;

    while(!open.empty()) {
        State current = open.top().first;
        open.pop();
        ++count_visited;
        if (is_goal(current)) 
            return {reconstruct_path(parent, current), count_visited};

        closed.insert(current);
        int current_g_score = g_score[current];
        int empty_idx = find_empty(current);
        auto moves = get_possible_moves(empty_idx);
        for (int target_idx : moves) {
            State next = make_move(current, empty_idx, target_idx);
            if (closed.find(next) == closed.end()) {
                int next_g_score = current_g_score + 1;
                auto it = g_score.find(next);
                if (it == g_score.end() || next_g_score < it->second) {
                    parent[next] = current;
                    g_score[next] = next_g_score;
                    int next_est = next_g_score + est(next);
                    open.push({next, next_est});
                }
            }
        }
    }

    return {std::vector<State>(), count_visited};
}