from __future__ import annotations
import sys
from os import chdir
from pathlib import Path
from collections import namedtuple
from math import log10, ceil, floor

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

# Storage for the range of integers
Range = namedtuple("Range", ["start", "end"])
ranges:list[Range] = []

# Parse the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        ranges_raw:list[str] = line.strip().split(",")
        ranges.extend(Range(int(x), int(y)) for x, y in (item.split("-") for item in ranges_raw if item))

def count_digits(n:int) -> int:
    """Counts how many digits an integer has."""
    if not isinstance(n, int) and (n < 0):
        raise ValueError("The input of 'count_digits()' must be an integer and not negative.")
    if (n > 0):
        return int(ceil(log10(n+1)))    # Adding 1 gets around the edge case of 'n' being a power of 10
    else: # (n == 0)
        return 1

# Macros for the directions in which invalid values are going to be searched
DIR_LEFT = "left"
DIR_RIGHT = "right"

def find_invalid(n:int, direction:str) -> tuple[int,int]:
    """
    Finds the nearest invalid value on the specified direction ('left' or 'right').
    If already at an invalid value, then the value itself is the nearest.
    
    Returns a tuple of integers, with the value itself and the sequence that repeats twice:
    (nearest_invalid_value, sequence_that_repeats)
    """

    if (n < 0):
        raise ValueError("The range must not include negative values.")

    direction = direction.strip().lower()
    
    # Amount of digits
    digits = count_digits(n)

    # If the amount of digits is odd, change 'n' to the next integer with an even amount of digits 
    if (digits % 2 == 1):
        if direction == DIR_RIGHT:
            n = 10**(digits)
            digits += 1
        elif direction == DIR_LEFT:
            n = (10**(digits - 1)) - 1
            digits -= 1

    # Mask for splitting the integer 'n' into two halves with the same amount of digits
    mask = 10**(digits // 2)

    # Left and right halves of the integer 'n'
    left, right = divmod(n, mask)

    # In case the right half's value is beyond the left half's value,
    # the right half has to "wrap around" in order to achieve the same value as the left half.
    if direction == DIR_RIGHT:
        if right > left: left += 1
    elif direction == DIR_LEFT:
        if right < left: left -= 1

    # Both halves have to be equal so the value is invalid,
    # so we are adding or subtracting 1 on the right half until it becomes equal to the left half.
    right = left

    # Reconstruct the integer from both halves
    invalid = (left * mask) + right

    return invalid, right

# Answer for Part 1
p1_answer:int = 0

for start, end in ranges:
    assert end >= start
    # Find the first invalid value after the range's start
    first_invalid, half_1 = find_invalid(start, DIR_RIGHT)
    
    # If the found value is outside the range, then the range has no invalid values
    if first_invalid > end: continue

    # Find the last invalid value before the range's end
    last_invalid, half_2 = find_invalid(end, DIR_LEFT)

    # Count the amount of invalid values within the range
    count = (half_2 - half_1) + 1

    # Calculate the sum of all values in the range (using the arithmetic series formula)
    p1_answer += int((first_invalid + last_invalid) * count / 2)

print(f"Part 1: {p1_answer}")
