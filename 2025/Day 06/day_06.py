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
    print("Instructions: Place a file named \"input.txt\" in the script's working directory, or run the script with the input file path as its only argument.", file=sys.stderr)
    sys.exit(1)

# The input lines with their characters in reverse order (needed for Part 2)
reversed_lines = []

# Container for the integers and the operations (for Part 1)
p1_problems = []    # For Part 1

# Dictionary of operations
operations = {"+": sum, "*": prod}

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        if not line.startswith(("+", "*")):
            # The integers to perform the operations on
            p1_problems.append(tuple(int(n) for n in line.split()))
        else:
            # The operations to perform with the integers
            # (we are attaching the sum and product functions from the standard library)
            p1_problems.append(tuple(operations[op] for op in line.split()))

        # Store a generator for the raw line with characters in reverse order (needed for Part 2)
        reversed_lines.append(reversed(line.strip("\n")))


### --- Part One --- ###

p1_problems = tuple(p1_problems)

p1_answer = 0

# Loop simultaneously through all the tuples of values and operations
# ('item' contains the elements with the same index across all tuples)
for item in zip(*p1_problems):
    operation = item[-1]    # The operation to perform is on the last element
    p1_values = item[:-1]      # The integers to operate on are all the elements except the last
    p1_answer += operation(p1_values)  # Perform the operation with the values

print(f"Part 1: {p1_answer}")

### --- Part Two --- ###

p2_answer = 0
p2_values = []

# Loop simultaneously through all the characters of all reversed lines
# ('item' contains the characters with the same index across all lines)
for item in zip(*reversed_lines):
    oper = item[-1]             # The operation is on the last line
    val = "".join(item[:-1])    # Join the characters of all lines but the last to form the value
    if val and not val.isspace():
        # Convert the value string to an integer if it is not empty or only blank spaces
        p2_values.append(int(val))
    if oper in ("+", "*"):
        # When the operator is found, perform its operation on the values
        p2_answer += operations[oper](p2_values)
        # Delete all elements from the list of values
        p2_values.clear()

print(f"Part 2: {p2_answer}")
