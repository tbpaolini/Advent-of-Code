from __future__ import annotations
from typing import Dict
from copy import deepcopy

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
    
    def __deepcopy__(self, memo=None):
        return Cave(self.name)
    
    def add_joint(self, node:Cave) -> None:
        self.joints.add(node)
        node.joints.add(self)

for point_a, point_b in nodes:
    cave_a = Cave(point_a)
    cave_b = Cave(point_b)
    cave_a.add_joint(cave_b)


def is_sublist(list1:list, list2:list) -> bool:
    size = len(list1)
    for i in range(len(list2) - len(list1) + 1):
        if list2[i : size+i] == list1: return True
    else:
        return False

def find_path(caves:dict):
    paths = set()
    max_visited = 0

    def travel(position:Cave, my_path:list=[]):
        nonlocal paths, max_visited

        if (position == "start") and (len(my_path) > 1): return

        if position == "end":
            paths.add(tuple(my_path + [position]))
            return
        
        my_exits = position.joints

        for next_cave in my_exits:
            if (next_cave.type == "small") and (next_cave in my_path):
                continue
            
            if len(my_path) > 1:
                node = [my_path[-1], position]
                if is_sublist(node, my_path):
                    continue

            travel(next_cave, my_path + [position])

    travel(caves["start"])
    return paths

routes = find_path(Cave.cave_system)
print(f"Part 1: {len(routes)}")

def print_paths(paths):
    for path in paths:
        text = ", ".join(cave.name for cave in path)
        print(text)

# Part 1:
# 39444 - too high
# 25066 - too high
#  1843 - too low

# Part 2:
# 15020 - too low
# 23786 - too low