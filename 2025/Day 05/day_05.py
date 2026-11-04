import sys
from os import chdir
from pathlib import Path
from collections import namedtuple

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
values = []

# Process the input
with open(Path(INPUT_FILENAME), "r", encoding="utf-8") as input_file:
    for line in input_file:
        if line.startswith("\n"): break
        start, end = line.strip().split("-")
        ranges.append(Range(int(start), int(end)))

    for line in input_file:
        if line.startswith("\n"): break
        values.append(int(line))

# Merging the overlapping ranges together
# Logic:
#   - Sort the ranges in ascending order by the starting value.    
#   - Keep a "current range" register, starting with the first range, and compare it to each of the ranges.
#   - If the current range overlaps with the next range, make the current range's end the maximum among both range's ends.
#   - If there is no overlap, add the current range the list of merged ranges, and make the next range the current one.

ranges.sort()   # Sorts ascendingly by the first element (the start of the range)
current_range = ranges[0]
ranges_merged:list[Range] = []

for next_range in ranges:
    if (current_range.start <= next_range.end) and (current_range.end >= next_range.start):
        # The current range overlaps with the next range
        current_range = Range(current_range.start, max(current_range.end, next_range.end))
    else:
        # There is no overlap with the next range
        ranges_merged.append(current_range)
        current_range = next_range

if current_range is not None:
    ranges_merged.append(current_range)

### --- Part One --- ###

p1_answer = 0

# Perform a binary search in the sorted list of non-overlapping ranges,
# in order to check if any of the ranges contain the value.
for val in values:
    # Boundaries of the search
    left = 0
    right = len(ranges_merged) - 1

    # Continue searching until the boundaries cross each other
    while left <= right:
        # Check the middle point between the boundaries
        middle = (left + right) // 2
        start, end = ranges_merged[middle]
        if val < start:
            right = middle - 1
        elif val > end:
            left = middle + 1
        else: # start <= val <= end
            p1_answer += 1  # Value is contained in the range
            break

print(f"Part 1: {p1_answer}")