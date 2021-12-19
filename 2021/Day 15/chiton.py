import numpy as np
from random import randint

# Work in progress (I will implement a proper pathfinding algorithm)

with open("input.txt", "rt") as file:
    cave_map = []
    for line in file: cave_map.append([int(char) for char in line.rstrip()])
    cave_map = np.array(cave_map, dtype="uint32")

def path_find(cave_map:np.ndarray):
    end_y, end_x = cave_map.shape
    cave_map = np.pad(cave_map, pad_width=1, mode="constant", constant_values=10)
    x = 0
    y = 0
    score = cave_map[y, x]

    while (y, x) != (end_y, end_x):
        choice_1 = cave_map[y, x+1]
        choice_2 = cave_map[y+1, x]

        if choice_1 < choice_2:
            x += 1
        elif choice_2 < choice_1:
            y += 1
        else:
            if randint(0, 1) == 0:
                x += 1
            else:
                y += 1
        
        score += cave_map[y, x]

    return score

min_score = 1000
for _ in range(1000):
    min_score = min(path_find(cave_map), min_score)

print(min_score)