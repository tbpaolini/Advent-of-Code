from itertools import product
from collections import namedtuple
import re
from time import perf_counter
from typing import Set, Tuple

input_regex = re.compile(r"(?m)(on|off) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)..(-?\d+),z=(-?\d+)..(-?\d+)")
instructions = []
Instruction = namedtuple("Instruction", "turn start end")
Coordinate = namedtuple("Coordinate", "x y z")

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 22\input.txt") as file:
    for line in file:
        turn, x1, x2, y1, y2, z1, z2 = input_regex.match(line).groups()
        start = Coordinate(int(x1), int(y1), int(z1))
        end   = Coordinate(int(x2), int(y2), int(z2))
        instructions.append(
            Instruction(turn, start, end)
        )
