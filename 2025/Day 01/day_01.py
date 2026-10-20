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

p1_answer = 0
value = 50

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        if line[0] == "R":
            # Add the number if the line starts with "R"
            value += int(line[1:])
        elif line[0] == "L":
            # Subtract the number if the line starts with "L"
            value -= int(line[1:])
        else:
            print(f"ERROR: Invalid line in the input ({line.strip()}).", file=sys.stderr)
            sys.exit(2)

        # Take the modulo to get where the dial lands (the dial has 100 numbers)
        # Important: Python already wraps the result to 99 when subtracting 1 from 0,
        #            other languages might not have this modulo behavior.
        value = value % 100
        if value == 0:
            p1_answer += 1

print(f"Part 1: {p1_answer}")
