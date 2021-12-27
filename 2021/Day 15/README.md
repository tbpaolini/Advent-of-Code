# Solution - Advent of Code 2021, Day 15 (using `A*` and `Dijkstra’s` algorithm)

This puzzle took me long because it was my first time with pathfinding algorithms, and I needed to learn the basics and also figure out how to implement. I am going to explain how it works, maybe this can help others in future who stumble upon this puzzle.

When looking for hints about this puzzle the two algorithms, the terms "A* Algorithm" and "Dijkstra’s Algorithm" popped up a lot, so that gave me a direction on what to search for. In the end I have implemented both, but my implementation of A* ended up faster than than mine of Dijkstra. Both give the correct results for the test input and Part 1, so I got right their basic premises. I ended up using only A* for Part 2.

Both of those are algorithms for traversing a graph, that is, a series of nodes joined by paths. They aim to find the best route between two points. Here is how A* works:

* You have two lists, one for "open" nodes and other for "closed" nodes.
* OPEN has the new nodes that you found, but haven't visited yet. It is a priority queue, where the nodes that are deemed to be the "best" choices come first.
* CLOSED store the nodes that were already visited.
* You begin by adding the starting point to the OPEN list, associated with the cost to get there (which for our puzzle is 0). And set it as the "current node".
* You look at all nodes linked to it that weren't visited yet. Then you calculate the cost to get to them. For our puzzle, the cost is the risk of the spot on the map. So you just add the cost so far to the value on those nodes.
* Then you store those new nodes on the OPEN list, associated with their total costs to get there.
* The current node is added to the CLOSED list.
* Now you pop from the OPEN list the node with the lowest cost, and set it as the "current node".
* And you keep repeating the process: check for the total cost of the linked unvisited nodes, add those to "OPEN", add the current node to CLOSED, pop the node from OPEN with the lowest cost, and set it as the "current node".
* The process end when the "current node" is the goal node. Its total cost will be the lowest possible cost to get there.

In a more general case, you can also associate each node on OPEN with the path to get to them, which is the list of nodes traversed to get there. This way you can reconstruct the path. However in our puzzle we are just concerned with the total cost to get there.

The Dijkstra’s Algorithm is somewhat similar, but it uses a different logic to decide which nodes to check first. Here is how it works:

* You have a list of UNVISITED nodes.
* You have a list of the COSTS to get to each node. Initially those values are set to "infinity", but in practice it can be just some very large arbitrary number.
* You begin by adding the cost of the starting node to the COSTS list, which for our puzzle is 0. You set it as the "current node".
* You check all nodes (not just the unvisited ones) that are linked to it, and calculate the total cost to get to each from the current node (for each linked node, just add the total cost so far with their respective cost).
* You compare those results with the ones stored on the COSTS list. For each linked node, if its new cost is smaller than the stored node, then you replace the cost on the list for the new one.
* Remove the current node from the UNVISITED list.
* Set the unvisited node with the lowest cost as the "current node".
* Keep repeating the process for the current node: calculate the costs of all linked nodes, update their respective costs if the results are smaller, then move to the unvisited node with the smallest cost.
* The process ends when there are no more unvisited nodes. At this point, the COSTS list will have the optimal cost to get on each node from the starting node. Just check the cost on the goal node.

I think that the bottleneck on my case was the choice of data structures to represents all those elements on those two algorithms. Python examples that I saw online usually used nested dictionaries to associate each node with its links and costs. I just used a NumPy array. I needed to calculate the links on each step (add or subtract 1 to each coordinate), then retrieve its values. I guess that it can be faster if you first scan the array for all nodes, check all their links, and store the results in a dictionary. At least on my tests, a 2D NumPy array showed to be faster than nested list.

It is worth noting that for the priority queues I used Python's `heapq` module, which is a built-in. It works on Python lists and turn them into "heap queues". Basically a heap queue is a list in which the elements are roughly ordered in ascending order. It tends to be reliable for getting the element with the lowest value, but it isn't always guaranteed to the absolute lowest. I assume here that the speed of the implementation, when compared to a regular sorted list, is enough to justify the "close enough" result.

Keep in mind that those pathfinding algorithms are a new thing to me, so what I did were certainly not the cleanest and most optimized implementations. But they work, and hopefully can help others to get started with the subject. Advent of Code is also supposed to be a learning experience :)

**Code:**

* [Parts 1 and 2 with A*](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2015/chiton.py)
* [Part 1 with Dijkstra](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2015/chiton_part2.py)

**Visualization:**

* [Plots of the cave map with the risks](https://github.com/tbpaolini/Advent-of-Code/tree/master/2021/Day%2015/plots)

**Further reading:**

* [The Insider’s Guide to A* Algorithm in Python](https://www.pythonpool.com/a-star-algorithm-python/)
* [Implementing Dijkstra’s Algorithm in Python](https://www.pythonpool.com/dijkstras-algorithm-python/)
* [The Python heapq Module: Using Heaps and Priority Queues](https://realpython.com/python-heapq-module/)