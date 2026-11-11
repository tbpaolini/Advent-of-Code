import sys
from os import chdir
from pathlib import Path

# Change the working directory to the directory containing this script
chdir(Path(__file__).absolute().parent)

# Default input file
INPUT_FILENAME = "input.txt"

# Use the first command-line argument as the input file
if len(sys.argv) == 2:
    INPUT_FILENAME = sys.argv[1]

# Check whether the input file exists
if (len(sys.argv) > 2) or (not Path(INPUT_FILENAME).exists()):
    print("Instructions: Place a file named \"input.txt\" in the script's working directory, or run the script with the input file path as its only argument.", file=sys.stderr)
    sys.exit(1)

board:list[list[str]] = []

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        board.append([char for char in line.strip()])

height = len(board)
width = len(board[0])

# All rows must have the same width
assert all(len(board[i]) == width for i in range(1, width))

# Tracks how many timelines each tachyon carries.
# It has the same dimensions as the board, with every cell initialized to zero.
# The tachyon at the source carries one timeline.
# When a tachyon hits a splitter, its timelines are added to the cells on either side of the splitter in the next row.
# When a tachyon continues straight down, its timelines are added to the cell below.
# The total number of timelines is the sum of the values in the last row.
timelines:list[list[int]] = [[0 for i in range(width)] for j in range(height)]

SOURCE = "S"
SPLITTER = "^"
TACHYON = "|"
EMPTY = "."

### --- Part One and Part Two --- ###

splits = 0

for y, row in enumerate(board[:-1]):
    for x, cell in enumerate(row):
        if cell not in (TACHYON, SOURCE): continue
        if cell == SOURCE:
            # If we are at the source, add a tachyon below.
            board[y+1][x] = TACHYON
            timelines[y+1][x] += 1
        if cell == TACHYON:
            if board[y+1][x] in (EMPTY, TACHYON):
                # If the cell below is empty or already contains a tachyon, add a tachyon below.
                board[y+1][x] = TACHYON
                timelines[y+1][x] += timelines[y][x]
            elif board[y+1][x] == SPLITTER:
                splits += 1
                # If the cell below contains a splitter, add a tachyon on each side.
                if (x-1 >= 0):
                    board[y+1][x-1] = TACHYON
                    timelines[y+1][x-1] += timelines[y][x]
                if (x+1 < width):
                    board[y+1][x+1] = TACHYON
                    timelines[y+1][x+1] += timelines[y][x]

# # Print the board
# for row in board:
#     print("".join(row))
# print()

print(f"Part 1 = {splits}")
print(f"Part 2: {sum(timelines[-1])}")
