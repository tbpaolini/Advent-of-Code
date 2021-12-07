with open("input.txt", "rt") as file:
    values = [int(n) for n in file.readline().split(",")]

# Part 1

def fuel(pos):
    return sum(abs(n - pos) for n in values)

max_pos = max(values)
min_pos = min(values)

result = fuel(max_pos)
for i in range(min_pos, max_pos):
    result = min(fuel(i), result)

print(f"Part 1: {result}")

# Part 2

def arithmetic_progression(n):
    return int((1 + n) * (n / 2))

def fuel2(pos):
    return sum(
        arithmetic_progression(abs(n - pos))
        for n in values
    )

result2 = fuel2(max_pos)
for i in range(min_pos, max_pos):
    result2 = min(fuel2(i), result2)

print(f"Part 2: {result2}")