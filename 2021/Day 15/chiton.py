from __future__ import annotations
from collections import namedtuple
from heapq import heappop, heappush
from time import perf_counter

Node = namedtuple("Node", "x y")
Path = namedtuple("Path", "risk node")

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 15\input.txt", "rt") as file:
    cave_map = []
    for line in file: cave_map.append([int(char) for char in line.rstrip()])

def path_find(cave_map:list[list[int]], start:Node, goal:Node) -> int:
    """Implementation of the A* algorithm to find the least risk path between
    two nodes in the cave"""

    # Risk of the starting position
    start_risk = cave_map[start.y][start.x]
    
    # Nodes that are known but weren't visited yet,
    # and their minimum risk to get there
    not_visited:list[Path] = [Path(start_risk, start)]
    
    # Nodes that were already visited
    visited:list[Node] = []

    while not_visited:
        
        # Get the known unvisited node with the least risk
        risk, node = heappop(not_visited)

        # Stop if we have arrived at the goal
        if node == goal: return risk - start_risk

        # Do not go to the node if we have already visited it
        if node in visited: continue

        # Coordinates of our current position
        x, y = node

        # Movement choices from our current position
        choices = (x+1, y), (x-1, y), (x, y+1), (x, y-1)
        
        for next_x, next_y in choices:

            # The destination must be within the map boundaries
            if (0 <= next_x <= max_x) and (0 <= next_y <= max_y):
                
                # Calculate the risk to the next node
                next_risk = risk + cave_map[next_y][next_x]
                next_node = Node(next_x, next_y)
                heappush(not_visited, Path(next_risk, next_node))
            
        visited += [node]

# Part 1

max_y = len(cave_map) - 1
max_x = len(cave_map[0]) - 1

start_time_part1 = perf_counter()
risk_part1 = path_find(cave_map, Node(0,0), Node(max_x, max_y))
total_time_part1 = perf_counter() - start_time_part1

print(f"Part 1: {risk_part1} (took {total_time_part1:.1f} seconds)")