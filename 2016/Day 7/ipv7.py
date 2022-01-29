from __future__ import annotations

from attr import field

def has_abba(text:str) -> bool:
    for i in range(len(text) - 3):
        substring = text[i : i+4]
        if (substring == substring[::-1]) and (substring[0] != substring[1]):
            return True
    else:
        return False

adresses = []
with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2016\Day 7\input.txt", "rt") as file:
    for line in file:
        assert line[-2].isalpha()
        fields = line.rstrip().replace("[", "]").split("]")
        adresses.append(fields)

# Part 1

count_p1 = 0
for ip in adresses:
    size = len(ip)
    outside = any(has_abba(ip[i]) for i in range(size) if i % 2 == 0)
    inside = all(not has_abba(ip[i]) for i in range(size) if i % 2 == 1)
    if outside and inside:
        count_p1 += 1

print(f"Part 1: {count_p1}")

# Part 2

def get_aba(text:str) -> set[str]:
    results = set()
    
    for i in range(len(text) - 2):
        substring = text[i : i+3]
        if (substring == substring[::-1]) and (substring[0] != substring[1]):
            results.add(substring)
    
    return results

count_p2 = 0
for ip in adresses:
    aba = set()
    bab = set()
    for j, _net in enumerate(ip):
        if j % 2 == 0: aba.update(get_aba(_net))
        if j % 2 == 1: bab.update(get_aba(_net))
    
    bab = {letter[1] + letter[0] + letter[1] for letter in bab}
    if len(aba & bab) > 0: count_p2 += 1

print(f"Part 2: {count_p2}")