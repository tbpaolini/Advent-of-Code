from math import sin, cos, radians
import numpy as np

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2016\Day 1\input.txt", "rt") as file:
    raw_line = file.readline().rstrip()
    values = [(value[0], int(value[1:])) for value in raw_line.split(", ")]

# Part 1

x = 0
y = 0
angle = 0

for rot, dist in values:
    if   rot == "R": angle += 90
    elif rot == "L": angle -= 90
    rad = radians(angle)

    x += round(dist * cos(rad))
    y += round(dist * sin(rad))

total_distance = abs(x) + abs(y)
print(total_distance)

# Part 2

x = 0
y = 0
angle = 0
visited = {(0, 0)}
hq_distance = None

