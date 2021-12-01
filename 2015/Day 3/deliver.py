from collections import Counter
from itertools import cycle

with open("input.txt", "rt") as file:
    move_set = file.read().strip()

# Part 1

x = 0
y = 0
house_counter = Counter()
house_counter.update({(x, y): 1})

for move in move_set:

    if move == "^":
        y += 1
    elif move == "v":
        y -= 1
    elif move == "<":
        x -= 1
    elif move == ">":
        x += 1
    
    house_counter.update({(x, y): 1})

print(len(house_counter))

# Part 2

class Gifter():
    def __init__(self) -> None:
        self.x = 0
        self.y = 0

santa = Gifter()
robot = Gifter()

turn = cycle((santa, robot))

house_counter2 = Counter()
house_counter2.update({(0, 0): 2})

for move in move_set:
    gifter = next(turn)

    if move == "^":
        gifter.y += 1
    elif move == "v":
        gifter.y -= 1
    elif move == "<":
        gifter.x -= 1
    elif move == ">":
        gifter.x += 1
    
    house_counter2.update({(gifter.x, gifter.y): 1})

print(len(house_counter2))