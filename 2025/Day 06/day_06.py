import sys
from os import chdir
from pathlib import Path
from math import prod

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

# Container for the integers and the operators
problems = []

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        if not line.startswith(("+", "*")):
            # The integers to perform the operations on
            problems.append(tuple(int(n) for n in line.split()))
        else:
            # The operations to perform with the integers
            # (we are attaching the sum and product functions from the standard library)
            problems.append(tuple((sum if op == "+" else prod) for op in line.split()))


### --- Part One --- ###

problems = tuple(problems)

p1_answer = 0

# Loop simultaneously through all the tuples of values and operations
# ('item' contains the elements with the same index across all tuples)
for item in zip(*problems):
    operation = item[-1]    # The operation to perform is on the last element
    values = item[:-1]      # The integers to operate on are all the elements except the last
    p1_answer += operation(values)  # Perform the operation with the values

print(f"Part 1: {p1_answer}")
