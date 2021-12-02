commands = []
with open("input.txt", "rt") as file:
    for line in file:
        cmd = line.strip().split()
        commands.append((cmd[0], int(cmd[1])))

# Part 1
depth = 0
horizontal = 0

for command, value in commands:
    if command == "down":
        depth += value
    elif command == "up":
        depth -= value
    elif command == "forward":
        horizontal += value

print(depth * horizontal)

# Part 2

depth = 0
horizontal = 0
aim = 0

for command, value in commands:
    if command == "down":
        aim += value
    elif command == "up":
        aim -= value
    elif command == "forward":
        horizontal += value
        depth += aim * value

print(depth * horizontal)