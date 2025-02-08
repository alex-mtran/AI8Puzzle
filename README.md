# AI8Puzzle
This is an AI project delving into three search algorithms on solving the 8 puzzle.

What is the 8 puzzle?
The 8 puzzle is a sliding tiles puzzle played on a 3x3 grid. The goal is to solve the 8 puzzle in as few moves as possible; the empty tile (denoted in this project as 0) must slide around until the 3x3 grid has the tiles in order from 1-8 from left to right, top to bottom, with the empty tile at the bottom right corner in as few moves as possible.

Search algorithms:
Uniform Cost Search -
performs a search in which chooses its next path to follow based on the lowest path costs presented at that node (g(n)). For this 8 puzzle, as each path will have the same cost of 1, this is the same as breadth first search.

A* with the Misplaced Tile heuristic -
performs a search in which chooses its next path to follow based on the f(n).
Misplaced Tile heuristic: the count of the number of tiles that are not in their correct position.

A* with the Manhattan Distance heuristic -
performs a search in which chooses its next path to follow based on the f(n).
Manhattan Distance heuristic: the aggregate count of the manhattan distances of each tile's current position and their correct position.
Manhattan distance is calculated as: |x1 - x2| + |y1 - y2|.

NOTE:
f(n) = h(n) + g(n) <=> final score = heuristic estimate til goal state + cost of path thus far
