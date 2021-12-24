from __future__ import annotations
from collections import namedtuple
from operator import le as less_than_or_equal
from operator import ge as greater_than_or_equal
import re
from typing import NamedTuple

input_regex = re.compile(r"(?m)(on|off) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)..(-?\d+),z=(-?\d+)..(-?\d+)")
Instruction = namedtuple("Instruction", "turn start end")
Coordinate = namedtuple("Coordinate", "x y z")
Vertex = namedtuple("Vertex", "min max")
instructions:list[Instruction] = []

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 22\input.txt") as file:
    for line in file:
        turn, x1, x2, y1, y2, z1, z2 = input_regex.match(line).groups()
        start = Coordinate(int(x1), int(y1), int(z1))
        end   = Coordinate(int(x2), int(y2), int(z2))
        instructions.append(
            Instruction(turn, start, end)
        )

class Cuboid:
    def __init__(self, start:Coordinate, end:Coordinate) -> None:
        self.cluster:set[Vertex] = {Vertex(start, end)}
    
    @classmethod
    def empty(cls) -> Cuboid:
        """Alternate constructor for getting an empty cuboid."""
        self = super().__new__(cls)
        self.cluster = set()
        return self
    
    def intersect(self, other:Vertex) -> dict[str, set[Vertex]]:
        """Checks which cuboids in the cluster overlap with another cuboid,
        and returns a dictionary with the full and partial overlaps."""
        
        result:dict[str, set[Vertex]] = dict(
            full =  set(),
            partial = set()
        )

        for myself in self.cluster:

            # Check whether the new cuboid fully encloses the old one
            # (All the new coordinates of the lower corner need to be smaller than the respective old ones,
            # and all new coordinates of the upper corner need to be bigger than the old.)
            lower_vertex_is_inside = all(map(less_than_or_equal, other.min, myself.min))
            upper_vertex_is_inside = all(map(greater_than_or_equal, other.max, myself.max))
            enclosed = lower_vertex_is_inside and upper_vertex_is_inside

            # If the cuboid is fully enclosed, add it to the dictionary and mode to the next one
            if enclosed:
                result["full"].add(myself)
                continue

            # Check for a partial overlap
            intersect = other.min.x <= myself.max.x and other.max.x >= myself.min.x and other.min.y <= myself.max.y and other.max.y >= myself.min.y and other.min.z <= myself.max.z and other.max.z >= myself.min.z
            if intersect:
                result["partial"].add(myself)
            
        return result
    
    def split(self, other_cuboids:Cuboid) -> None:
        """Break the cuboid into cuboids that do not intersect with the input."""
        
        # Loop through all cuboids of the cluster being checked against
        for other in other_cuboids.cluster:

            # Check for intersections
            intersections = self.intersect(other)

            # Split the cuboids that are partially intersected
            # (the regions outside the intersection become new cuboids)
            new_cuboids = set()
            for myself in intersections["partial"]:
                
                # Below x-axis (yz-plane)
                if other.min.x in range(myself.min.x+1, myself.max.x+1):
                    
                    # Exclude the intersecting part from the old cuboid
                    old_min = Coordinate(myself.min.x, myself.min.y, myself.min.z)
                    old_max = Coordinate(other.min.x - 1, myself.max.y, myself.max.z)
                    
                    # Include the part outside the intersection
                    new_min = Coordinate(other.min.x, myself.min.y, myself.min.z)
                    new_max = Coordinate(myself.max.x, myself.max.y, myself.max.z)
                    new = Vertex(new_min, new_max)
                    
                    myself = Vertex(old_min, old_max)
                    new_cuboids.add(new)
                
                # Above x-axis (yz-plane)
                if other.max.x in range(myself.min.x, myself.max.x):
                    
                    # Exclude the intersecting part from the old cuboid
                    old_min = Coordinate(other.max.x + 1, myself.min.y, myself.min.z)
                    old_max = Coordinate(myself.max.x, myself.max.y, myself.max.z)
                    
                    # Include the part outside the intersection
                    new_min = Coordinate(myself.min.y, myself.min.y, myself.min.z)
                    new_max = Coordinate(other.max.x, myself.max.y, myself.max.z)
                    new = Vertex(new_min, new_max)
                    
                    myself = Vertex(old_min, old_max)
                    new_cuboids.add(new)
                    
                ####
                    
                # Below y-axis (xz-plane)
                if other.min.y in range(myself.min.y+1, myself.max.y+1):
                    
                    # Exclude the intersecting part from the old cuboid
                    old_min = Coordinate(myself.min.x, myself.min.y, myself.min.z)
                    old_max = Coordinate(myself.max.x, other.min.y - 1, myself.max.z)
                    
                    # Include the part outside the intersection
                    new_min = Coordinate(myself.min.x, other.min.y, myself.min.z)
                    new_max = Coordinate(myself.max.x, myself.max.y, myself.max.z)
                    new = Vertex(new_min, new_max)
                    
                    myself = Vertex(old_min, old_max)
                    new_cuboids.add(new)
                
                # Above y-axis (xz-plane)
                if other.max.y in range(myself.min.y, myself.max.y):
                    
                    # Exclude the intersecting part from the old cuboid
                    old_min = Coordinate(myself.min.x, other.max.y + 1, myself.min.z)
                    old_max = Coordinate(myself.max.x, myself.max.y, myself.max.z)
                    
                    # Include the part outside the intersection
                    new_min = Coordinate(myself.min.y, myself.min.y, myself.min.z)
                    new_max = Coordinate(myself.max.x, other.max.y, myself.max.z)
                    new = Vertex(new_min, new_max)
                    
                    myself = Vertex(old_min, old_max)
                    new_cuboids.add(new)

                ####

                # Below z-axis (xy-plane)
                if other.min.z in range(myself.min.z+1, myself.max.z+1):
                    
                    # Exclude the intersecting part from the old cuboid
                    old_min = Coordinate(myself.min.x, myself.min.y, myself.min.z)
                    old_max = Coordinate(myself.max.x, myself.max.y, other.min.z - 1)
                    
                    # Include the part outside the intersection
                    new_min = Coordinate(myself.min.x, myself.min.y, other.min.z)
                    new_max = Coordinate(myself.max.x, myself.max.y, myself.max.z)
                    new = Vertex(new_min, new_max)
                    
                    myself = Vertex(old_min, old_max)
                    new_cuboids.add(new)
                
                # Above z-axis (xy-plane)
                if other.max.z in range(myself.min.z, myself.max.z):
                    
                    # Exclude the intersecting part from the old cuboid
                    old_min = Coordinate(myself.min.x, myself.min.y, other.max.z + 1)
                    old_max = Coordinate(myself.max.x, myself.max.y, myself.max.z)
                    
                    # Include the part outside the intersection
                    new_min = Coordinate(myself.min.y, myself.min.y, myself.min.z)
                    new_max = Coordinate(myself.max.x, myself.max.y, other.max.z)
                    new = Vertex(new_min, new_max)
                    
                    myself = Vertex(old_min, old_max)
                    new_cuboids.add(new)
            
            # Remove from the cluster cuboids that intersect
            self.cluster -= intersections["full"]
            self.cluster -= intersections["partial"]
            
            # Add the new (non-intersecting) cuboids to the cluster
            self.cluster |= new_cuboids
    
    def __add__(self, other:Cuboid):
        """Addition of cuboids. The new areas are added to the cluster."""
        self.split(other)
        self.cluster |= other.cluster
        return self

    def __sub__(self, other:Cuboid):
        """Subtraction of cuboids. The intersecting areas are removed from the cluster."""
        self.split(other)
        return self
    
    def volume(self):
        """Calculate the volume of the cuboid cluster."""
        result = 0
        for cuboid in self.cluster:
            result += ((cuboid.max.x - cuboid.min.x + 1) * (cuboid.max.y - cuboid.min.y + 1) * (cuboid.max.z - cuboid.min.z + 1))
        return result

result = Cuboid.empty()
for turn, start, end in instructions:
    if turn == "on":
        result = result + Cuboid(start, end)
    elif turn == "off":
        result = result - Cuboid(start, end)
print(result.volume())
print(len(result.cluster))

# https://old.reddit.com/r/adventofcode/comments/rlxhmg/2021_day_22_solutions/hpmgqn5/
# 1268313839428137
#   56139851991767
#  158273978831275
# 1559111472001834
# 1184279658831399