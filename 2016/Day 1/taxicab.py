from math import sin, cos, radians
import numpy as np
from itertools import product

with open("input.txt", "rt") as file:
    raw_line = file.readline().rstrip()
    values = [(value[0], int(value[1:])) for value in raw_line.split(", ")]

x = 0
y = 0
angle = 0
history = []
overlaps = []

for rot, dist in values:
    if   rot == "R": angle += 90
    elif rot == "L": angle -= 90
    rad = radians(angle)

    x_start = x
    y_start = y
    x_dir = round(cos(rad))
    y_dir = round(sin(rad))

    x += dist * x_dir
    y += dist * y_dir
    x_range = range(x_start, x, x_dir) if x_dir != 0 else (x_start,)
    y_range = range(y_start, y, y_dir) if y_dir != 0 else (y_start,)

    for coord in product(x_range, y_range):
        if coord in history: overlaps += [coord]
        history += [coord]

total_distance = abs(x) + abs(y)
print(f"Part 1: {total_distance}")

hq_distance = abs(overlaps[0][0]) + abs(overlaps[0][1])
print(f"Part 2: {hq_distance}")