from __future__ import annotations
from collections import deque
from itertools import combinations, repeat
from typing import List

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 12\input.txt", "rt") as file:
    nodes = []
    for line in file:
        nodes += [tuple(line.rstrip().split("-"))]

class Cave():

    cave_system = {}

    def __new__(cls:Cave, name:str) -> Cave:

        # Does an instance of the cave already exists?
        if name in cls.cave_system:
            return cls.cave_system[name]    # Return the existing instance
        else:
            self = super().__new__(cls)
            cls.cave_system[name] = self    # Create a new instance
            
            self.name  = name
            if name in ("start", "end"): self.type = name
            elif name.isupper(): self.type = "big"
            elif name.islower(): self.type = "small"
            self.joints = set()

            return self
    
    def __eq__(self, obj:object) -> bool:
        return self.name == obj
    
    def __hash__(self) -> int:
        return hash(self.name)
    
    def __repr__(self) -> str:
        return f"Cave('{self.name}')"
    
    def add_joint(self, node:Cave) -> None:
        self.joints.add(node)
        node.joints.add(self)

for point_a, point_b in nodes:
    cave_a = Cave(point_a)
    cave_b = Cave(point_b)
    cave_a.add_joint(cave_b)


def find_path(caves:dict):
    paths = set()
    max_visited = 0

    def travel(position:Cave, my_path:list=[], step=0):
        nonlocal paths, max_visited

        if position == "end":
            num_visited = 0
            for cave in set(my_path):
                if cave.type == "small": num_visited += 1
            if num_visited < max_visited: return
            if num_visited > max_visited: paths.clear()
            max_visited = num_visited
            paths.add(tuple(my_path + [position]))
            return
        
        previous_paths = split_list(my_path, position)
        for path_a, path_b in combinations(previous_paths, r=2):
            if is_sublist(path_a, path_b):
                return
        
        my_exits = position.joints - {position}
        for next_cave in my_exits:
            if (next_cave.type == "small") and (next_cave in my_path):
                continue
            else:
                travel(next_cave, my_path + [position], step + 1)

    travel(caves["start"])
    return paths

test = find_path(Cave.cave_system)

# 39444 - too high
# 25066 - too high