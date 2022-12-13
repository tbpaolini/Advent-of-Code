## [Day 12 of 2022](https://adventofcode.com/2022/day/12)

In order to represent the mountain map, I used a 2-D array of nodes (each node is a struct with the elevation and pointers to the exits).

I used the [Dijkstra's algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm) in order to find the best path between the start and the end. The algorithm keeps track of which nodes were visited so far, the current node, and the minimum cost so far to get to each node from the start. In our case, the 'cost' is the amount of steps needed to reach the node.

The Dijkstra's algorithm works like this:

1. Initialize the minimum cost of all nodes to `infinity`, except the starting node (initialized to 0). In practice, "infinity" can be just any very large number.
2. On the `current node`, calculate the cost to get to all of its exits (the cost of the current node plus the cost to get to the exit, which in our case it is just adding 1).
3. For each exit, check if its calculated cost to it is smaller the minimum cost stored on the exit. If it is, then update the node's minimum cost to the new cost.
4. Mark the `current node` as `visited`.
5. Among all unvisited nodes, set the `current node` to the node with the smallest cost (in case more than one node has the same cost, it can be any of those; but if you want, you can prioritize the one among them that is closest to the destination).
6. Repeat steps `2` to `5` until the destination node `destination node` is marked as `visited`.

At that point, the cost of the cost on the destination node is the cost of the optimal path to get there from the start. If that cost is `infinity`, then it means that there is no path from the start to the destination.

The ideal way to use Dijkstra is having a priority queue for the unvisited nodes. But I had already spent a lot of time to get the pathfinding to work, so in order to simplify things I made a list of nodes that were "seen" but not yet "visited". Then I checked for the smallest cost in that list.

Looking at other solutions, I saw that people managed to solve the puzzle with simpler pathfinding algorithms. You might want to have a look at the `Bellman-Ford algorithm`, the `Breadth-first search` (BFS), or the `A* search`. BFS, in particular, is pretty much Dijkstra without a priority queue (the nodes are visited in the order they are first seen).

**My solution:** [day_12.c](./day_12.c) (finishes in *120 ms* on my old laptop, when compiled with `-O3`)