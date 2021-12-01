with open("input.txt") as file:
    instructions = file.readline().strip()

# Puzzle 1

floor = 0
for char in instructions:
    if char == "(":
        floor += 1
    elif char == ")":
        floor -= 1

print(floor)

# Puzzle 2

floor = 0
for index, char in enumerate(instructions, start=1):
    if char == "(":
        floor += 1
    elif char == ")":
        floor -= 1
    
    if floor == -1:
        print(index)
        break