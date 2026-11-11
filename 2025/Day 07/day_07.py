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
    print("Instructions: Place a file named \"input.txt\" in the script's working directory, or run the script with the input file path as its only argument.", file=sys.stderr)
    sys.exit(1)

board:list[list[str]] = []

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        board.append([char for char in line.strip()])

height = len(board)
width = len(board[0])

# All rows must have the same size
assert all(len(board[i]) == width for i in range(1, width))

SOURCE = "S"
SPLITTER = "^"
TACHYON = "|"
EMPTY = "."

### --- Part One --- ###

splits = 0

for y, row in enumerate(board[:-1]):
    width = len(row)
    for x, cell in enumerate(row):
        if cell not in (TACHYON, SOURCE): continue
        if cell == SOURCE:
            # If we are at the source, add a tachyon below
            board[y+1][x] = TACHYON
        if cell == TACHYON:
            if board[y+1][x] == EMPTY:
                # If there is an empty space below, just add a tachyon below
                board[y+1][x] = TACHYON
            elif board[y+1][x] == SPLITTER:
                splits += 1
                # If we have a splitter below, add a tachyon on each side of the splitter
                if (x-1 >= 0):
                    board[y+1][x-1] = TACHYON
                if (x+1 < width):
                    board[y+1][x+1] = TACHYON

# # Print the board
# for row in board:
#     print("".join(row))
# print()

print(f"Part 1 = {splits}")
