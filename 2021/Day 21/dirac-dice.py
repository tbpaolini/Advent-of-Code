from itertools import cycle, count
from collections import Counter

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 21\input.txt", "rt") as file:
    position = {}
    for player, line in enumerate(file, start=1):
        position[player] = int(line.split()[-1]) - 1

def deterministic_dice(sides:int, count:int) -> int:
    sides = cycle(range(1, sides+1))
    while True: yield sum(next(sides) for _ in range(count))

score = Counter()
d100 = deterministic_dice(100, 3)
players = cycle(position)
dice_count = count(3, 3)

for player, roll, dice_count in zip(players, d100, dice_count):
    position[player] = (position[player] + roll) % 10
    score.update({player: position[player] + 1})
    if score[player] >= 1000: break

min_score = min(score.values())
print(min_score * dice_count)