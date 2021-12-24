import re
from collections import namedtuple
from operator import le as less_than_or_equal

input_regex = re.compile(r"(?m)(on|off) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)..(-?\d+),z=(-?\d+)..(-?\d+)")
Instruction = namedtuple("Instruction", "turn cuboid")
Coordinate = namedtuple("Coordinate", "x y z")
Cuboid = namedtuple("Cuboid", "min max")
instructions:list[Instruction] = []

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 22\test_input.txt") as file:
    for line in file:
        turn, x1, x2, y1, y2, z1, z2 = input_regex.match(line).groups()
        start = Coordinate(int(x1), int(y1), int(z1))
        end   = Coordinate(int(x2), int(y2), int(z2))
        my_cuboid = Cuboid(start, end)
        instructions.append(
            Instruction(turn, my_cuboid)
        )

core = []

for turn, new in instructions:
    new:Cuboid
    new_core = []

    for old in core:
        old:Cuboid

        # Two cuboids intersect when the minimum coordinate of one is smaller
        # than the maximum coordinate of the other, for all 3 axes
        intersect = all(map(less_than_or_equal, new.min, old.max)) and all(map(less_than_or_equal, old.min, new.max))

        # If there is no overlap between the old and new blocks,
        # then the old block will still remain on regardless of the instruction
        if not intersect:
            new_core += [old]
            continue

        # x-axis
        if old.min.x <= new.max.x <= old.max.x:  # Positive direction
            new_core += [Cuboid(Coordinate(new.max.x+1, old.min.y, old.min.z), Coordinate(old.max.x, old.max.y, old.max.z))]
            old = Cuboid(Coordinate(old.min.x, old.min.y, old.min.z), Coordinate(new.max.x, old.max.y, old.max.z))

        if old.min.x <= new.min.x <= old.max.x:  # Negative direction
            new_core += [Cuboid(Coordinate(old.min.x, old.min.y, old.min.z), Coordinate(new.min.x-1, old.max.y, old.max.z))]
            old = Cuboid(Coordinate(new.min.x, old.min.y, old.min.z), Coordinate(old.max.x, old.max.y, old.max.z))

        # y-axis
        if old.min.x <= new.max.x <= old.max.x:  # Positive direction
            new_core += [Cuboid(Coordinate(old.min.x, new.max.y+1, old.min.z), Coordinate(old.max.x, old.max.y, old.max.z))]
            old = Cuboid(Coordinate(old.min.x, old.min.y, old.min.z), Coordinate(old.max.x, new.max.y, old.max.z))

        if old.min.x <= new.min.x <= old.max.x:  # Negative direction
            new_core += [Cuboid(Coordinate(old.min.x, old.min.y, old.min.z), Coordinate(old.max.x, new.min.y-1, old.max.z))]
            old = Cuboid(Coordinate(old.min.x, new.min.y, old.min.z), Coordinate(old.max.x, old.max.y, old.max.z))

        # z-axis
        if old.min.x <= new.max.x <= old.max.x:  # Positive direction
            new_core += [Cuboid(Coordinate(old.min.x, old.min.y, new.max.z+1), Coordinate(old.max.x, old.max.y, old.max.z))]
            old = Cuboid(Coordinate(old.min.x, old.min.y, old.min.z), Coordinate(old.max.x, old.max.y, new.max.z))

        if old.min.x <= new.min.x <= old.max.x:  # Negative direction
            new_core += [Cuboid(Coordinate(old.min.x, old.min.y, old.min.z), Coordinate(old.max.x, old.max.y, new.min.z-1))]
            old = Cuboid(Coordinate(old.min.x, old.min.y, new.min.z), Coordinate(old.max.x, old.max.y, old.max.z))
    
    if turn == "on": new_core += [new]
    else: assert turn == "off"
    core = new_core

print(len(core))

volume = 0
for block in core:
    # assert block.max.x >= block.min.x
    # assert block.max.y >= block.min.y
    # assert block.max.z >= block.min.z
    volume += (block.max.x - block.min.x + 1) * (block.max.y - block.min.y + 1) * (block.max.z - block.min.z + 1)

print(volume)

# 1268313839428137
# 1166383171984860