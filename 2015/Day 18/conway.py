import numpy as np
from itertools import product

grid = np.zeros(shape=(100, 100), dtype="int8")

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2015\Day 18\input.txt", "rt") as file:
    for y, line in enumerate(file):
        for x, value in enumerate(line.strip()):
            if value == "#":
                grid[x, y] = 1

while True:
    try:
        part = int(input("Part (1 or 2): ").strip())
        if part in (1, 2):
            break
    except ValueError:
        continue

if part == 2:
    grid[0, 0] = grid[0, 99] = grid[99, 0] = grid[99, 99] = 1

for i in range(100):
    temp_grid = grid.copy()

    for x,y in product(range(100), repeat=2):

        my_value = grid[x, y]
        x1 = max(x-1, 0)
        y1 = max(y-1, 0)
        x2 = min(x+2, 100)
        y2 = min(y+2, 100)
        
        my_sum = np.sum(grid[x1 : x2, y1 : y2]) - my_value
        assert my_value == 0 or my_value == 1
        assert my_sum >= 0 and my_sum <= 8
        
        if my_value == 1:
            if my_sum != 2 and my_sum != 3:
                temp_grid[x, y] = 0
        else:
            if my_sum == 3:
                temp_grid[x, y] = 1
        
    grid = temp_grid
    if part == 2:
        grid[0, 0] = grid[0, 99] = grid[99, 0] = grid[99, 99] = 1
        
    board = "\n\n"
    for y in range(100):
        line = "".join("#" if value else "." for value in grid[..., y])
        board += line + "\n"
    board +=f"Gen {i+1}: {np.count_nonzero(grid)}"
    print(board, end="")

final_count = np.count_nonzero(grid)
print(f"\n\nPart {part}: {final_count}")