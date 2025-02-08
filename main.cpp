#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <chrono>

using namespace std;

// GLOBAL VARIABLES
// explicit goal state
const vector<vector<int>> GOAL_STATE = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}}; // can be hardcoded to expand or shrink to nxn puzzles

// potential operators
const vector<pair<int, int>> MOVES = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // operators for all sliding tiles puzzles

struct Node {
    vector<vector<int>> state;
    int g, h; // g = cost, h = heuristic
    int expanded_nodes_count; // keep track of expanded nodes count
    Node* parent;

    Node(vector<vector<int>> s, int g_cost, int h_cost, Node* p) 
        : state(s), g(g_cost), h(h_cost), parent(p) {}

    int f() const { return g + h; }

    bool operator>(const Node& other) const {
        return f() > other.f(); // overloaded in order to expand lowest f(n) node first
    }
};





// FUNCTION DECLARATIONS
int misplaced_tiles(const vector<vector<int>>& state);
int manhattan_distance(const vector<vector<int>>& state);
vector<Node*> expand(Node* node, int heuristic);
pair<int, int> find_blank(const vector<vector<int>>& state);
void print_solution(Node* node);
Node* general_search(const vector<vector<int>>& initial_state, int heuristic);





// FUNCTION DEFINITIONS
/* general_search(problem, QUEUEING-FUNCTION) {
    nodes = MAKE-QUEUE(MAKE-NODE(problem.INITIAL_STATE))
    loop do
        if EMPTY(nodes) then return "failure"
            node = REMOVE-FRONT(nodes)
        if problem.GOAL-TEST(node.STATE succeeds then return node
            nodes = QUEUEING-FUNCTION(nodes, EXPAND(node, problem.OPERATORS))
        end
    }
*/

Node* general_search(const vector<vector<int>>& initial_state, int heuristic) { // heuristic == 1 for uniform cost search, heuristic == 2 for misplaced tiles; heuristic == 3 for manhattan distance
    // nodes = MAKE-QUEUE(MAKE-NODE(problem.INITIAL_STATE))
    using NodeComparator = function<bool(Node*, Node*)>;
    NodeComparator x = [](Node* a, Node* b) { return *a > *b; }; // expands lowest f score first
    using NodePriorityQueue = priority_queue<Node*, vector<Node*>, NodeComparator>;

    NodePriorityQueue nodes(x); // nodes = MAKE-QUEUE
    
    int h_cost = (heuristic == 1) ? 0 : (heuristic == 2) ? misplaced_tiles(initial_state) : manhattan_distance(initial_state);
    Node* root = new Node(initial_state, 0, h_cost, nullptr); // MAKE-NODE(problem.INITIAL_STATE)
    nodes.push(root);

    int expanded_nodes = -1; // count of expanded nodes over the entirety of the algorithm // == -1 so that expanded nodes does not account for the initial state as an expanded node

    auto start_time = std::chrono::high_resolution_clock::now();

    while (!nodes.empty()) { // loop do
        Node* node = nodes.top();
        nodes.pop();

        expanded_nodes++;

        if (node->state == GOAL_STATE) {
            auto end_time = std::chrono::high_resolution_clock::now();
            cout << "Time taken (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << endl;
            cout << "Count of nodes expanded: " << expanded_nodes << endl;
            return node; // success!
        }
        // nodes = QUEUEING-FUNCTION(nodes, EXPAND(node, problem.OPERATORS))
        for (Node* child : expand(node, heuristic)) { // EXPAND(node, problem.OPERATORS)
            nodes.push(child);
        }
    }

    return nullptr; // if EMPTY(nodes) then return "failure"
}

// misplaced tile heuristic
// returns count of misplaced tiles
int misplaced_tiles(const vector<vector<int>>& state) {
    int count = 0;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (state[r][c] == 0) continue; // heuristic does not take into account the blank (0) tile
            if (state[r][c] != GOAL_STATE[r][c]) count++;
        }
    }
    return count;
}

// manhattan distance heuristic
// returns the summation of the manhattan distances from every tile (does not take into account the blank (0) tile) to its correct placement
// NOTE: should modify to only calculate distances from misplaced tiles instead of every tile for more efficiency but 3x3 grid space is so small it doesn't really matter at this point
// target = state[r][c] - 1 because the actual calculation for the correct row and column needs the value -1
// ex: for tile 6 (state[r][c] = 6), goal_row = (6-1) / 3 = 1 (correct output) where as 6 / 3 = 2 (incorrect output); similar for goal_col
int manhattan_distance(const vector<vector<int>>& state) {
    int distance = 0;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (state[r][c] == 0) continue; // heuristic does not take into account the blank (0) tile
            int target = state[r][c] - 1; 
            int goal_row = target / 3, goal_col = target % 3; // use 3 here because there are 3 rows and 3 columns
            distance += abs(r - goal_row) + abs(c - goal_col); // |(current position) - (correct position)|
        }
    }
    return distance;
}

// generate child nodes by moving the blank tile
vector<Node*> expand(Node* node, int heuristic) {
    vector<Node*> children;
    auto [zero_row, zero_col] = find_blank(node->state);

    for (const auto& move : MOVES) {
        int new_row = zero_row + move.first, new_col = zero_col + move.second;
        if (new_row >= 0 && new_row < 3 && new_col >= 0 && new_col < 3) { // actual operators // checker to prevent invalid moves (when blank (0) tile is along a boundary)
            vector<vector<int>> new_state = node->state;
            swap(new_state[zero_row][zero_col], new_state[new_row][new_col]); // swaps blank (0) tile with the tile in the new position

            int h_cost = (heuristic == 1) ? 0 : (heuristic == 2) ? misplaced_tiles(new_state) : manhattan_distance(new_state);
            children.push_back(new Node(new_state, node->g+1, h_cost, node));
        }
    }
    return children;
}

// returns the position of the blank (0) tile
pair<int, int> find_blank(const vector<vector<int>>& state) {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (state[r][c] == 0) return {r, c};
        }
    }
    return {-1, -1}; // error
}

// prints the solution path
void print_solution(Node* node) {
    vector<vector<vector<int>>> path;
    int count = -1; // == -1 so that root node (initial state) counts as depth 0

    while (node) { // stores node states starting from goal state back to initial state
        path.push_back(node->state);
        node = node->parent;
        count++;
    }
    
    cout << "Depth of solution: " << count << endl;

    reverse(path.begin(), path.end()); // reorders so that print shows solution path from initial state to goal state
    for (const auto& state : path) {
        for (const auto& row : state) {
            for (int num : row) {
                cout << to_string(num) << ' ';
            }
            cout << endl;
        }
        cout << "---------" << endl;
    }
}

int main() {
    cout << "Enter the number of the puzzle configuration you would like to choose to begin solving the 8 sliding tiles puzzle!" << endl;
    cout << "1) Trivial puzzle" << endl;
    cout << "2) Default easy puzzle" << endl;
    cout << "3) Default medium puzzle" << endl;
    cout << "4) Default hard puzzle" << endl;
    cout << "5) Custom puzzle" << endl;

    int puzzle_choice;
    cin >> puzzle_choice;

    vector<vector<int>> initial_state(3, vector<int>(3));

    if (puzzle_choice == 1) { // trivial
        initial_state = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 0}
        };
    }
    else if (puzzle_choice == 2) { // easy
        initial_state = {
            {1, 2, 3},
            {4, 5, 6},
            {0, 7, 8}
        };
    }
    else if (puzzle_choice == 3) { // medium
        initial_state = {
            {1, 2, 3},
            {4, 0, 5},
            {6, 7, 8}
        };
    }
    else if (puzzle_choice == 4) { // hard
        initial_state = {
            {2, 7, 3},
            {4, 6, 8},
            {1, 5, 0}
        };
    }
    else if (puzzle_choice == 5) {
        cout << "Enter your custom puzzle for a 3x3 puzzle problem. Please only enter valid configurations. (enter values left to right, row by row, separated by spaces) (use '0' for the blank tile) (hit ENTER after filling in the values): ";
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                cin >> initial_state[i][j];
            }
        }
    }
    else {
        cout << "Invalid choice. Default puzzle will be chosen." << endl;
        initial_state = {
            {1, 2, 3},
            {4, 0, 5},
            {6, 7, 8}
        };
    }

    cout << "Enter the number of the algorithm that you would like to use to solve this puzzle." << endl;
    cout << "1) Uniform cost search" << endl;
    cout << "2) A star using misplaced tile heuristic" << endl;
    cout << "3) A star using manhattan distance heuristic" << endl;

    int algo_choice;
    cin >> algo_choice;

    if (algo_choice == 1) {
        cout << "Uniform Cost Search:" << endl;
        Node* ucs = general_search(initial_state, algo_choice);
        if (ucs) print_solution(ucs);
    }
    else if (algo_choice == 2) {
        cout << "A* (Misplaced Tile Heuristic):" << endl;
        Node* astar_misplaced = general_search(initial_state, algo_choice);
        if (astar_misplaced) print_solution(astar_misplaced);
    }
    else if (algo_choice == 3) {
        cout << "A* (Manhattan Distance Heuristic):" << endl;
        Node* astar_manhattan = general_search(initial_state, algo_choice);
        if (astar_manhattan) print_solution(astar_manhattan);
    }
    else {
        cout << "Invalid choice. Exiting application." << endl;
        return -1;
    }

    return 0;
}