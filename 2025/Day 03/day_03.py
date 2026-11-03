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

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    banks = tuple(tuple(int(c) for c in line.strip()) for line in input_file if line)

### --- Part One --- ###

p1_answer = 0

for bank in banks:

    # The first digit is the largest one in the sequence, excluding the last.
    max_value_1 = 0
    max_index_1 = 0
    for i, n in enumerate(bank[:-1]):
        if n > max_value_1:
            max_index_1, max_value_1 = i, n
            if max_value_1 == 9: break

    # The second digit is the largest one in the remaining sequence.
    max_value_2 = 0
    max_index_2 = 0
    for i, n in enumerate(bank[max_index_1 + 1:]):
        if n > max_value_2:
            max_index_2, max_value_2 = i, n
            if max_value_2 == 9: break

    # Construct the "joltage" value.
    p1_answer += (max_value_1 * 10) + max_value_2

print(f"Part 1: {p1_answer}")

### --- Part Two --- ###

def max_index_and_value(batteries:tuple[int, ...]) -> tuple[int, int]:
    """Given a tuple of integers, return the index of the maximum value and that value."""
    
    max_index = 0
    max_value = batteries[0]

    for i, n in enumerate(batteries[1:]):
        if n > max_value:
            max_index, max_value = i+1, n   # It is 'i+1' because we are skipping the first element

    return max_index, max_value

DIGITS = 12     # Amount of "joltage" digits for Part 2
p2_answer = 0

for bank in banks:
    assert len(bank) >= DIGITS
    start_index = 0 # Starting position in the array
    joltage = 0

    # Loop through all digits of the "joltage" value
    for d in range(DIGITS):
        # The array must have enough elements left for the remaining digits
        end_index = len(bank) - (DIGITS - 1) + d

        # Splice the array to the section being searched,
        # and find the maximum value and its index
        local_index, max_digit = max_index_and_value(bank[start_index : end_index])

        # Construct the "joltage" value by adding the digit to its position
        joltage += max_digit * (10**(DIGITS - d - 1))

        # Move the starting position to the next element
        start_index += local_index + 1

    # Add the "joltage" value to the answer
    p2_answer += joltage

print(f"Part 2: {p2_answer}")
