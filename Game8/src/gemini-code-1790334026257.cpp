std::pair<std::vector<State>, int> BFS_solve(State init_state) {
    if (is_goal(init_state)) return {{init_state}, 0};

    // Вспомогательные функции для упаковки/распаковки состояний
    auto pack = [](const State& s) -> uint64_t {
        uint64_t res = 0;
        for (int i = 0; i < 16; ++i) {
            res |= (static_cast<uint64_t>(s[i]) << (i * 4));
        }
        return res;
    };

    auto unpack = [](uint64_t val) -> State {
        State s(16);
        for (int i = 0; i < 16; ++i) {
            s[i] = (val >> (i * 4)) & 0xF;
        }
        return s;
    };

    // Битовый сдвиг вместо медленного выделения вектора в make_move
    auto make_move_packed = [](uint64_t state, int empty_idx, int target_idx) -> uint64_t {
        uint64_t target_shift = target_idx * 4;
        uint64_t empty_shift = empty_idx * 4;
        uint64_t target_val = (state >> target_shift) & 0xF;
        
        state &= ~(0xFULL << target_shift); // Очищаем клетку
        state |= (target_val << empty_shift); // Перемещаем значение
        return state;
    };

    int count_visited = 0;
    std::queue<uint64_t> open;
    // Хранит упакованное состояние и индекс пустой клетки у родителя (1 байт)
    std::unordered_map<uint64_t, uint8_t> parent_move; 

    uint64_t init_packed = pack(init_state);
    open.push(init_packed);
    parent_move[init_packed] = 255; // Метка начального состояния

    uint64_t goal_packed = pack(GOAL);
    uint64_t found_goal = 0;
    bool found = false;

    while (!open.empty()) {
        uint64_t current_packed = open.front();
        open.pop();
        ++count_visited;

        // Поиск пустой клетки напрямую в битовом представлении
        int empty_index = 0;
        for (int i = 0; i < 16; ++i) {
            if (((current_packed >> (i * 4)) & 0xF) == 0) {
                empty_index = i;
                break;
            }
        }

        std::vector<int> moves = get_possible_moves(empty_index);

        for (auto target_idx : moves) {
            uint64_t next_packed = make_move_packed(current_packed, empty_index, target_idx);

            if (parent_move.find(next_packed) == parent_move.end()) {
                parent_move[next_packed] = static_cast<uint8_t>(empty_index);
                open.push(next_packed);
                
                if (next_packed == goal_packed) {
                    found_goal = next_packed;
                    found = true;
                    break;
                }
            }
        }
        if (found) break;
    }

    if (!found) return {std::vector<State>(), count_visited};

    // Восстановление пути через обратные битовые операции
    std::vector<State> path;
    uint64_t curr_packed = found_goal;
    
    while (true) {
        path.push_back(unpack(curr_packed));
        uint8_t parent_empty = parent_move[curr_packed];
        if (parent_empty == 255) break;
        
        int curr_empty = 0;
        for (int i = 0; i < 16; ++i) {
            if (((curr_packed >> (i * 4)) & 0xF) == 0) {
                curr_empty = i;
                break;
            }
        }
        curr_packed = make_move_packed(curr_packed, curr_empty, parent_empty);
    }
    
    std::reverse(path.begin(), path.end());
    return {path, count_visited};
}