import sys
from os import chdir
from pathlib import Path

# Change the current working directory to be the same directory as this script
chdir(Path(__file__).absolute().parent)

# Default input file
INPUT_FILENAME = "input.txt"

# Set the input file to the first argument of the script
if len(sys.argv) == 2:
    INPUT_FILENAME = sys.argv[1]

# Check if the input file exists
if (len(sys.argv) > 2) or (not Path(INPUT_FILENAME).exists()):
    print("Instructions: Please have a file called \"input.txt\" in the working directory of this script or run the script with the input file's path as its only parameter.", file=sys.stderr)
    sys.exit(1)

# List of lists to represent a 2D array of booleans
grid:list[list[bool]] = []

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        grid.append([(True if char == "@" else False) for char in line.strip()])

MAX_NEIGHBORS = 3

def count_neighbors(bool_grid:list[list[bool]], y, x) -> int:
    """Counts the amount of neighbors of a cell in a 2D array of booleans, given a coordinate."""
    
    count = 0 if not bool_grid else -1  # Do not count the element on the cell itself
    height = len(bool_grid)
    
    # Loop through the 3 x 3 grid centered on the coordinate
    for yy in (y-1, y, y+1):
        if (yy < 0) or (yy >= height): continue # Check if we are inside the grid
        width = len(bool_grid[yy])
        for xx in (x-1, x, x+1):
            if (xx < 0) or (xx >= width):continue   # Check if we are inside the grid
            if bool_grid[yy][xx]: count += 1
    
    return count

### --- Part One --- ###

p1_answer = 0

for y, row in enumerate(grid):
    for x, cell in enumerate(row):
        if not cell: continue
        if count_neighbors(grid, y, x) <= MAX_NEIGHBORS: p1_answer += 1

print(f"Part 1: {p1_answer}")

