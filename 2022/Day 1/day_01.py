# Note: My actual solution is at the file 'day_01.c'
# I used this Python script to help me checking if I got correct the logic of my C code.

calories = []
elf = 0
with open("input.txt", "rt") as file:
    for line in file:
        if line != "\n":
            elf += int(line)
        else:
            calories.append(elf)
            elf = 0
calories.append(elf)

print(sorted(calories)[-3:])
print(sum(sorted(calories)[-3:]))