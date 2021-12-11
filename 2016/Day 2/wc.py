import numpy as np

numpad = np.arange(1, 10).reshape(3, 3)

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2016\Day 2\input.txt", "rt") as file:
    directions = [line.rstrip() for line in file]

# Part 1

row = 1
column = 1

def up():
    global row
    if row == 0: return
    row -= 1

def down():
    global row
    if row == 2: return
    row += 1

def left():
    global column
    if column == 0: return
    column -= 1

def right():
    global column
    if column == 2: return
    column += 1

handlers = dict(
    U = up,
    D = down,
    L = left,
    R = right
)

code = ""
for instruction in directions:
    for char in instruction:
        handlers[char]()
    code += str(numpad[row, column])

print(f"Part 1: {code}")

# Part 2

NaN = "NaN"
nextgen_numpad = np.array([
    [NaN, NaN, NaN, NaN, NaN, NaN, NaN],
    [NaN, NaN, NaN, "1", NaN, NaN, NaN],
    [NaN, NaN, "2", "3", "4", NaN, NaN],
    [NaN, "5", "6", "7", "8", "9", NaN],
    [NaN, NaN, "A", "B", "C", NaN, NaN],
    [NaN, NaN, NaN, "D", NaN, NaN, NaN],
    [NaN, NaN, NaN, NaN, NaN, NaN, NaN]
])

true_code = ""
row = 3
column = 1

for instruction in directions:
    for char in instruction:
        if char == "U":
            if nextgen_numpad[row-1, column] != NaN: row -= 1
        elif char == "D":
            if nextgen_numpad[row+1, column] != NaN: row += 1
        elif char == "L":
            if nextgen_numpad[row, column-1] != NaN: column -= 1
        elif char == "R":
            if nextgen_numpad[row, column+1] != NaN: column += 1
    
    true_code += nextgen_numpad[row, column]

print(f"Part 2: {true_code}")