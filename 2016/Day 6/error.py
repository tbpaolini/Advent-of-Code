from collections import Counter

count = [Counter() for _ in range(8)]
with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2016\Day 6\input.txt", "rt") as file:
    for line in file:
        for i in range(8):
            count[i].update(line[i])

message_p1 = ""
message_p2 = ""
for j in range(8):
    message_p1 += count[j].most_common()[0][0]
    message_p2 += count[j].most_common()[-1][0]

print(f"Part 1: {message_p1}")
print(f"Part 2: {message_p2}")