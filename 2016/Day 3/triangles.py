from itertools import combinations

with open("input.txt", "rt") as file:
    p1_values = []
    for line in file:
        p1_values.append(tuple(int(val) for val in line.split()))


p1_triangles = []

for sides in p1_values:
    total = sum(sides)
    
    for pair in combinations(sides, r=2):
        my_sum = sum(pair)
        if my_sum <= (total - my_sum): break
    else:
        p1_triangles += [sides]

print(f"Part 1: {len(p1_triangles)}")

# Part 2

p2_values = []
for i in range(0, len(p1_values), 3):
    group = p1_values[i : i+3]
    for j in range(3):
        p2_values += [tuple(val[j] for val in group)]

p2_triangles = []

for sides in p2_values:
    total = sum(sides)
    
    for pair in combinations(sides, r=2):
        my_sum = sum(pair)
        if my_sum <= (total - my_sum): break
    else:
        p2_triangles += [sides]

print(f"Part 2: {len(p2_triangles)}")