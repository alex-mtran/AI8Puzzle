#include <iostream>
#include <vector>
#include <queue>
#include <functional>

using namespace std;

// GLOBAL VARIABLES
// explicit goal state
const vector<vector<int>> GOAL_STATE = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}}; // can be hardcoded to expand or shrink to nxn puzzle problems

// potential operators
const vector<pair<int, int>> MOVES = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

struct Node {
    vector<vector<int>> state;
    int g, h; // g = cost, h = heuristic
    Node* parent;

    Node(vector<vector<int>> s, int g_cost, int h_cost, Node* p) 
        : state(s), g(g_cost), h(h_cost), parent(p) {}

    int f() const { return g + h; } // f(n) = g(n) + h(n) // path score = cost of path so far + estimated path left til goal state based on heuristic // the lower the better

    bool operator>(const Node& other) const {
        return f() > other.f(); // overloaded in order to expand lowest f(n) node first
    }
};

// FUNCTION DECLARATIONS
int misplaced_tiles(const vector<vector<int>>& state);
int manhattan_distance(const vector<vector<int>>& state);
// vector<Node*> expand(Node* node, int heuristic);
// initial_state_gen(int seed);

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

Node* general_search(const vector<vector<int>>& initial_state, int heuristic) { // heuristic == 1 for misplaced tiles; heuristic == 2 for manhattan distance
    // nodes = MAKE-QUEUE(MAKE-NODE(problem.INITIAL_STATE))
    using NodeComparator = function<bool(Node*, Node*)>;
    NodeComparator x = [](Node* a, Node* b) { return *a > *b; }; // expands lowest f score first
    using NodePriorityQueue = priority_queue<Node*, vector<Node*>, NodeComparator>;

    NodePriorityQueue nodes(x); // nodes = MAKE-QUEUE
    
    int h_cost = (heuristic == 1) ? misplaced_tiles(initial_state) : manhattan_distance(initial_state); // h-cost is cost of new state (new_state) // 1 for misplaced tiles heuristic, 2 for manhattan distance heuristic (and for uniform cost search)
    Node* root = new Node(initial_state, 0, h_cost, nullptr); // MAKE-NODE(problem.INITIAL_STATE)
    nodes.push(root);

    while (!nodes.empty()) { // loop do
        Node* node = nodes.top();
        nodes.pop();

        if (node->state == GOAL_STATE) return node; // success!

        // nodes = QUEUEING-FUNCTION(nodes, EXPAND(node, problem.OPERATORS))
        /*for (Node* child : expand(node, heuristic)) { // EXPAND(node, problem.OPERATORS) // UNFINISHED
            nodes.push(child);
        }*/
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
            int goal_row = target / 3, goal_col = target % 3; // use 3 herebecause there are 3 rows and 3 columns
            distance += abs(r - goal_row) + abs(c - goal_col); // |(current position) - (correct position)|
        }
    }
    return distance;
}

// generate child nodes by moving the blank tile
/*vector<Node*> expand(Node* node, int heuristic) {
    TODO;
}*/

int main() {
    
    // initial_state_gen(int seed);
	    // return only valid initial states (some initial states are impossible to solve)

    return 0;
}