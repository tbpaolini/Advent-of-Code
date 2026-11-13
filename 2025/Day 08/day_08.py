import sys
from os import chdir
from pathlib import Path
from typing import NamedTuple
from itertools import combinations
from heapq import nlargest
from math import prod

# Use the directory containing this script as the working directory.
chdir(Path(__file__).absolute().parent)

# Default input file.
INPUT_FILENAME = "input.txt"

# Puzzle parameters.
PAIRS_AMOUNT = 1000
LARGEST_CIRCUITS_AMOUNT = 3

# Use the first command-line argument as the input file, if provided.
if len(sys.argv) == 2:
    INPUT_FILENAME = sys.argv[1]

# Check that the input file exists and that no extra arguments were provided.
if (len(sys.argv) > 2) or (not Path(INPUT_FILENAME).exists()):
    print("Instructions: Place a file named \"input.txt\" in the script's working directory, or run the script with the input file path as its only argument.", file=sys.stderr)
    sys.exit(1)

class Coordinate(NamedTuple):
    """Coordinates of a point in 3D space."""
    x:int = 0
    y:int = 0
    z:int = 0

class Pair(NamedTuple):
    """A pair of points and the square of the distance between them."""
    start:Coordinate = Coordinate()
    end:Coordinate = Coordinate()
    distance_squared:int = 0

coords:list[Coordinate] = []
pairs:list[Pair] = []

# Parse the input coordinates.
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        if not line.startswith("\n"):
            x, y, z = line.strip().split(",")
            coords.append(Coordinate(int(x), int(y), int(z)))

def get_distance_squared(coord_1:Coordinate, coord_2:Coordinate) -> int:
    """Return the squared distance between two points in 3D space."""
    return (coord_1.x - coord_2.x)**2 + (coord_1.y - coord_2.y)**2 + (coord_1.z - coord_2.z)**2

# Generate every coordinate pair and sort them by squared distance.
# Square roots are unnecessary because they do not change the distance ordering.
pairs = sorted(
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

# Map each coordinate to its index in the disjoint-set arrays.
nodes:dict[Coordinate, int] = {point:i for i, point in enumerate(coords)}

# Parent index for each node. A node is a root when its parent is itself.
# 'nodes' maps each coordinate to an index; 'parents' uses those same indices.
# Thus, 'parents[N]' gives the parent index of the node whose index is 'N'.
parents = [i for i in range(len(nodes))]

# Number of other nodes in each component, stored at its root.
# Values at non-root indices are stale and must not be used.
# Also, 'children_count[N]' gives the children count of the node whose index is 'N'.
children_count = [0 for _ in range(len(nodes))]

def find_root(index:int):
    while parents[index] != index:
        # Point to the grandparent to shorten the path to the root.
        parents[index] = parents[parents[index]]
        index = parents[index]
    return index

# Number of separate components (circuits) remaining.
circuits_remaining = len(nodes)

# Process pairs in order of increasing distance, joining their components.
for step, (point_1, point_2, dist_sq) in enumerate(pairs):

    if step == PAIRS_AMOUNT:
        # The first PAIRS_AMOUNT pairs have been processed; report Part One.
        circuit_sizes:list[int] = []
        for i in range(len(nodes)):
            if parents[i] == i:
                circuit_sizes.append(children_count[i] + 1)
        largest_circuits = nlargest(LARGEST_CIRCUITS_AMOUNT, circuit_sizes)
        p1_answer = prod(largest_circuits)
        print(f"Part 1: {p1_answer}")
    
    # Get each point's index in the disjoint-set arrays.
    index_1 = nodes[point_1]
    index_2 = nodes[point_2]
    
    # Find the root of each point's component.
    root_index_1 = find_root(index_1)
    root_index_2 = find_root(index_2)

    # Skip pairs whose points are already in the same component.
    if root_index_1 == root_index_2:
        continue

    # Attach the second root beneath the first and add its component's nodes
    # to the first root's count (including the second root itself).
    children_count[root_index_1] += (1 + children_count[root_index_2])
    parents[root_index_2] = root_index_1
    circuits_remaining -= 1
    
    if circuits_remaining == 1:
        # All points are connected; report Part Two.
        p2_answer = point_1.x * point_2.x
        print(f"Part 2: {p2_answer}")
        break
