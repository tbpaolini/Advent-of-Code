import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path

input_path = Path(__file__).parents[1] / "input.txt"

with open(input_path, "rt") as file:
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

print("Saving images...")

fig, ax = plt.subplots(figsize=(16, 9))
ax.set_title("Cave Map")
sns.heatmap(cave_map, ax=ax)
fig.savefig("cave_map.svg")
fig.savefig("cave_map.png")
print("Saved cave map.")

fig, ax = plt.subplots(figsize=(16, 9))
ax.set_title("Risk Map")
sns.heatmap(risk_map, ax=ax)
fig.savefig("risk_map.svg")
fig.savefig("risk_map.png")
print("Saved risk map.")

fig, ax = plt.subplots(figsize=(16, 9))
ax.set_title("Distance Map")
sns.heatmap(distance_map, ax=ax)
fig.savefig("distance_map.svg")
fig.savefig("distance_map.png")
print("Saved distance map.")