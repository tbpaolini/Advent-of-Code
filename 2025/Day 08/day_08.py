import sys
from os import chdir
from pathlib import Path
from collections import namedtuple
from typing import NamedTuple
from itertools import combinations
from heapq import nsmallest, nlargest
from math import prod

# Change the current working directory to be the same directory as this script
chdir(Path(__file__).absolute().parent)

# Default input file
INPUT_FILENAME = "input.txt"

# Problem's parameters
PAIRS_AMOUNT = 1000
LARGEST_CIRCUITS_AMOUNT = 3

# Set the input file to the first argument of the script
if len(sys.argv) == 2:
    INPUT_FILENAME = sys.argv[1]

# Check if the input file exists
if (len(sys.argv) > 2) or (not Path(INPUT_FILENAME).exists()):
    print("Instructions: Place a file named \"input.txt\" in the script's working directory, or run the script with the input file path as its only argument.", file=sys.stderr)
    sys.exit(1)

class Coordinate(NamedTuple):
    """Coordinates of a point in a 3D space."""
    x:int = 0
    y:int = 0
    z:int = 0

class Pair(NamedTuple):
    """A pair of points in a 3D space."""
    start:Coordinate = Coordinate()
    end:Coordinate = Coordinate()
    distance_squared:int = 0

coords:list[Coordinate] = []
pairs:list[Pair] = []

# Parse the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        if not line.startswith("\n"):
            x, y, z = line.strip().split(",")
            coords.append(Coordinate(int(x), int(y), int(z)))

def get_distance_squared(coord_1:Coordinate, coord_2:Coordinate) -> int:
    """Calculates the square of the distance between two coordinates in a 3D space."""
    return (coord_1.x - coord_2.x)**2 + (coord_1.y - coord_2.y)**2 + (coord_1.z - coord_2.z)**2

# Get the 1000 pairs with the smallest with the smallest distance between them
# Note: It is not needed to calculate the square root of the distances because that would not affect the ordering
pairs = nsmallest(
    PAIRS_AMOUNT,
    (
        Pair(
            coord_1,
            coord_2,
            get_distance_squared(coord_1, coord_2)
        )
        for coord_1, coord_2 in combinations(coords, r=2)
    ),
    key = lambda pair: pair.distance_squared
)

# Create a tuple of unique coordinates in the 'pairs' arrays
unique_points:set[Coordinate] = set()
for pair in pairs:
    unique_points.add(pair.start)
    unique_points.add(pair.end)

# Dictionary that correlates an unique point to an unique index number (starting from zero, incrementing by one)
nodes:dict[Coordinate, int] = {point:i for i, point in enumerate(unique_points)}

# Array of parent nodes:
# - Each index corresponds to the same index in the 'nodes' dictionary
# - Each value represents the index of its parent node
# - When a value is equal to its own index, it means that the node is a root node (that is, it has no parent)
parents = [i for i in range(len(nodes))]

# Array for the amount of children nodes a root node has
# - Each index corresponds to the same index in the 'nodes' dictionary
# - The value represents the total amount of children its respective node
# - The value only has a meaning if the node is a root node (other values are disregarded)
children_count = [0 for _ in range(len(nodes))]

def find_root(index:int):
    while parents[index] != index:
        # parents[index] = parents[parents[index]]    # Flatten the current node
        index = parents[index]
    return index

# Link each of the pairs together (forming a graph)
for point_1, point_2, dist_sq in pairs:
    
    # Get the indexes of each point
    index_1 = nodes[point_1]
    index_2 = nodes[point_2]
    
    # Find the index of the root nodes for each point
    root_index_1 = find_root(index_1)
    root_index_2 = find_root(index_2)

    if root_index_1 == root_index_2:
        continue

    # Link together the root nodes of both points (root of point 2 becomes the child)
    children_count[root_index_1] += (1 + children_count[root_index_2])
    # parents[root_index_1] = root_index_1
    parents[root_index_2] = root_index_1

circuit_sizes:list[int] = []
for i in range(len(nodes)):
    if parents[i] == i:
        circuit_sizes.append(children_count[i] + 1)

largest_circuits = nlargest(LARGEST_CIRCUITS_AMOUNT, circuit_sizes)
p1_answer = prod(largest_circuits)

print(f"Part 1: {p1_answer}")
