import numpy as np

with open("input.txt", "rt") as file:
    dots = []
    for line in file:
        if line == "\n": break
        x, y = line.rstrip().split(",")
        dots += [(int(x), int(y))]
    
    instructions = []
    for line in file:
        my_instruction = line.split()
        my_instruction = my_instruction[-1].split("=")
        axis, pos = my_instruction
        instructions += [(axis, int(pos))]
    
max_x = max(x for x, y in dots)
max_y = max(y for x, y in dots)

paper = np.zeros(shape=(max_y+1, max_x+1), dtype="bool")
for x, y in dots:
    paper[y, x] = True

for step, (axis, pos) in enumerate(instructions):
    if axis == "x":
        half_1 = paper[..., 0:pos]
        half_2 = np.flip(paper[..., pos+1:], axis=1)
    
    elif axis == "y":
        half_1 = paper[0:pos, ...]
        half_2 = np.flip(paper[pos+1:, ...], axis=0)
    
    paper = half_1 | half_2
    
    if step == 0:
        dots_count = np.count_nonzero(paper)
        print(f"Part 1: {dots_count} dots")

output = ""
for row in paper:
    line = ""
    for dot in row: line += "o" if dot else " "
    output += line + "\n"

print(f"\nPart 2:\n{output}")