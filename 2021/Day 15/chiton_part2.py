from __future__ import annotations
from collections import namedtuple
from heapq import heappop, heappush
from time import perf_counter
import numpy as np

Node = namedtuple("Node", "x y")

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 15\input.txt", "rt") as file:
    cave_map = []
    for line in file: cave_map.append([int(char) for char in line.rstrip()])
    cave_map = np.array(cave_map)

def pathfind_dijkstra(cave_map:np.ndarray, start:Node, goal:Node) -> int:
    
    # The positions on the map that were visited
    visited = np.zeros_like(cave_map, dtype="bool")

    # The distances from the start to any node.
    # In the context of this puzzle, 'distance' is the total risk.
    # (initial values are set to 'infinity', which in our case
    # it is just the  biggest integer I can fit there)
    distance = np.full_like(cave_map, 2**32 - 1, dtype="uint32")

    # Heap queue to store the next positions to visit.
    # It a list of tuples, which the first element is the distance to the node
    # and the second is the node's coordinates.
    # The heap queue will return first the nodes with the smallest distance
    to_visit:list[tuple[int, Node]] = []

    # Starting position
    distance[start.y, start.x] = 0
    x = start.x
    y = start.y

    # Limits of the map
    y_max, x_max = cave_map.shape
    x_max -= 1
    y_max -= 1

    # Loop through the map until all positions are visited
    while not np.all(visited):
        
        # Get the adjascent nodes to the current position
        exits = Node(x+1, y), Node(x-1, y), Node(x, y+1), Node(x, y-1)

        # Calculate the distance to each adjascent node
        for node in exits:

            
            # The destination must be inside the map
            if (0 <= node.x <= x_max) and (0 <= node.y <= y_max):
                
                if visited[node.y, node.x]: continue
                
                # Calculate the distance between the starting position and the next node
                node_distance = distance[y, x] + cave_map[node.y, node.x]
                
                # If this distance is smaller than the one stored, replace it by the new distance
                if node_distance < distance[node.y, node.x]:
                    distance[node.y, node.x] = node_distance
                
                # Store the visit in the heap queue
                visit = (distance[node.y, node.x], Node(node.x, node.y))
                heappush(to_visit, visit)
        
        # Mark the current node as visited
        visited[y, x] = True
        print(np.count_nonzero(visited), end="\r")

        # Get the next node to visit
        next_distance, next_node = heappop(to_visit)
        x, y = next_node
    
    # After all nodes have been visited, the optimal distance is the one stored on the goal position
    return distance[goal.y, goal.x]

y_goal, x_goal = cave_map.shape
x_goal -= 1
y_goal -= 1
risk = pathfind_dijkstra(cave_map, Node(0, 0), Node(x_goal, y_goal))
print()
print(risk)