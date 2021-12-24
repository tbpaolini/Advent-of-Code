from itertools import product
from collections import namedtuple
import re
from time import perf_counter
from typing import Set, Tuple

"""NOTE
I made this solution for Part 1 before I saw Part 2, even though I already imagined it would ask for all cubes.
I had decided to keep simple at first, instead of assuming what would come later.
Needless to say, the solution for this part does not work for part 2 because it would reqeuire some inordinate
amount of memory. Please, see the file cubes2.py for the more elegant solution :-)
"""

input_regex = re.compile(r"(?m)(on|off) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)..(-?\d+),z=(-?\d+)..(-?\d+)")
instructions = []
Instruction = namedtuple("Instruction", "turn start end")
Coordinate = namedtuple("Coordinate", "x y z")

with open("input.txt") as file:
    for line in file:
        turn, x1, x2, y1, y2, z1, z2 = input_regex.match(line).groups()
        start = Coordinate(int(x1), int(y1), int(z1))
        end   = Coordinate(int(x2), int(y2), int(z2))
        instructions.append(
            Instruction(turn, start, end)
        )

def xyz_range(start, end, cutoff_start=(-50,-50,-50), cutoff_end=(50, 50, 50)) -> Set[Tuple[int, int]]:
    """Returns a set of integer coordidates (in a 3D space) inside
    the cuboid between points (x1, y1, z1) and (x2, y2, z2).
    And also within the cutoff limits."""

    x1, y1, z1 = start
    x2, y2, z2 = end
    x_min, y_min, z_min = cutoff_start
    x_max, y_max, z_max = cutoff_end

    x1 = max(x_min, x1)
    x2 = min(x_max, x2)
    y1 = max(y_min, y1)
    y2 = min(y_max, y2)
    z1 = max(z_min, z1)
    z2 = min(z_max, z2)

    # All combinations of coordinates within their respective ranges
    cuboid = product(
        range(x1, x2+1),
        range(y1, y2+1),
        range(z1, z2+1),
    )

    # Add the coordinates to a set and return it
    return set(cuboid)

start_time = perf_counter()
cubes_on = set()
for (turn, start, end) in instructions:
    if turn == "on":
        cubes_on |= xyz_range(start, end)
    
    elif turn == "off":
        cubes_on -= xyz_range(start, end)
    
    assert len(cubes_on) <= 1_030_301
total_time = perf_counter() - start_time

print(f"Part 1: {len(cubes_on)} (took {total_time:.3f} seconds)")