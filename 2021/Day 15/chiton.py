import numpy as np

with open("input.txt", "rt") as file:
    risk_map = []
    for line in file: risk_map.append([int(char) for char in line.rstrip()])
    risk_map = np.array(risk_map, dtype="uint32")

width, height = risk_map.shape

distance_map = np.fromfunction(
    function=(lambda y,x: width + height - 2 - x - y),
    shape=risk_map.shape,
    dtype=risk_map.dtype
)

cave_map = risk_map + distance_map

def path_find(cave_map:np.ndarray):
    end_y, end_x = cave_map.shape
    cave_map = np.pad(cave_map, pad_width=1, mode="constant", constant_values=10)
    x = 0
    y = 0
    score = risk_map[y, x]