from __future__ import annotations
from typing import Dict
from copy import deepcopy
from collections import Counter

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
    
    def __eq__(self, obj:object) -> bool:   # We can compare wheter two caves are the same
        return self.name == obj
    
    def __hash__(self) -> int:  # Each cave returns a unique hash so it can be used in a set or dictionary key
        return hash(self.name)
    
    def __repr__(self) -> str:  # User friendly representation on terminal
        return f"Cave('{self.name}')"
    
    def __deepcopy__(self, memo=None):  # Enable the deep copy feature
        return Cave(self.name)
    
    def add_joint(self, node:Cave) -> None:  # Join two caves together
        self.joints.add(node)
        node.joints.add(self)


# Create and join caves
for point_a, point_b in nodes:
    cave_a = Cave(point_a)
    cave_b = Cave(point_b)
    cave_a.add_joint(cave_b)

def find_path(caves:dict, extra_visit=False):
    paths = set()
    visit_count = Counter()

    # Recursively search through the paths
    def travel(position:Cave, my_path:list=[], has_extra_visit:bool=False, count:Counter=visit_count):
        nonlocal paths

        # Stop if we arrived at the start again
        if (position == "start") and (len(my_path) > 1): return

        # Stop once we reach the end
        if position == "end":
            paths.add(tuple(my_path + [position]))
            return
        
        # Count the visits on the current path
        my_count = deepcopy(count)
        if position.type == "small":
            my_count.update((position,))
            max_visits = 2 if has_extra_visit else 1
            visit_number = my_count[position]
            if visit_number == max_visits: has_extra_visit=False
            if visit_number > max_visits: return    # Stop if we reached the maximum visits of the small cave

        # Try each possible exit
        for next_cave in position.joints:
            travel(next_cave, my_path + [position], has_extra_visit, my_count)

    # Begin recursive search of all paths
    travel(caves["start"], has_extra_visit=extra_visit)
    return paths

routes_p1 = find_path(Cave.cave_system)
print(f"Part 1: {len(routes_p1)}")

routes_p2 = find_path(Cave.cave_system, True)
print(f"Part 2: {len(routes_p2)}")

# def print_paths(paths):
#     for path in paths:
#         text = ", ".join(cave.name for cave in path)
#         print(text)

# Part 1:
# 39444 - too high
# 25066 - too high
#  1843 - too low

# Part 2:
# 15020 - too low
# 23786 - too low