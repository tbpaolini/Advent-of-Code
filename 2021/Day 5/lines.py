import numpy as np
from collections import namedtuple

Line = namedtuple("Line", "x1 y1 x2 y2")
segments = []

# Parse the file
with open("input.txt", "rt") as file:
    for text_line in file:
        raw_xy = [text_line.split(",") for text_line in text_line.strip().split(" -> ")]
        (x1, y1), (x2, y2) = raw_xy
        segment = Line(int(x1), int(y1), int(x2), int(y2))
        segments.append(segment)

# Find the boundaries of the 2D space
max_x = max( max(line.x1, line.x2) for line in segments )
max_y = max( max(line.y1, line.y2) for line in segments )

# Create a 2D array to represent the space
grid = np.zeros(shape=(max_x+1, max_y+1), dtype="int16")

# Part 1

for x1, y1, x2, y2 in segments:
    # Whether the line is horizontal or vertical
    if (x1 == x2) or (y1 == y2):
        if (x1 < x2) or (y1 < y2):
            grid[x1 : x2+1, y1 : y2+1] += 1     # Left-to-right or top-to-bottom
        elif (x1 > x2) or (y1 > y2):
            grid[x2 : x1+1, y2 : y1+1] += 1     # Right-to-left or bottom-to-top

intersection_count = np.count_nonzero(grid > 1)
print(f"Part 1: {intersection_count}")

# Part 2

for x1, y1, x2, y2 in segments:
    if (x1 != x2) and (y1 != y2):

        # If the coordinates are decreasing or increasing
        x_dir = -1 if x1 > x2 else 1
        y_dir = -1 if y1 > y2 else 1
        
        # Return coordinate pairs, with each coordinate changing one unit at a time
        # (according to whether it is increasing or decreasing)
        xy_range = zip(
            range(x1, x2+x_dir, x_dir),
            range(y1, y2+y_dir, y_dir)
        )
        
        # On the grid: add one on each point
        for x, y in xy_range:
            grid[x, y] += 1

intersection_count = np.count_nonzero(grid > 1)
print(f"Part 2: {intersection_count}")