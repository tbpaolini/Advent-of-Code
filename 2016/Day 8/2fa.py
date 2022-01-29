from turtle import width
import numpy as np
import re
from PIL import Image

display = np.zeros((6, 50), dtype="bool")

def rect(display, x, y):
    display[0:y, 0:x] = True

def rotate_column(display, column, offset):
    display[..., column] = np.roll(display[..., column], offset)

def rotate_row(display, row, offset):
    display[row, ...] = np.roll(display[row, ...], offset)

operations = {
    "rect": rect,
    "rotate row": rotate_row,
    "rotate column": rotate_column,
}

input_regex = re.compile(r"(?m)(rect|rotate row|rotate column).+?(\d+).+?(\d+)")
with open("input.txt", "rt") as file:
    raw_input = file.read()

instructions = []
for match in input_regex.finditer(raw_input):
    command, arg1, arg2 = match.groups()
    instructions.append( ( command, int(arg1), int(arg2) ) )

# Parts 1 and 2

for command, arg1, arg2 in instructions:
    operations[command](display, arg1, arg2)

print(f"Part 1: {np.count_nonzero(display)}")
print("Part 2:")
pixels = ""
for row in display:
    pixels += "".join("█" if pixel else " " for pixel in row)
    pixels += "\n"
print(pixels)

display = np.pad(display, 1, "constant")
height, width = display.shape
display_image = Image.fromarray(display).resize((width*10, height*10), resample=Image.NEAREST)
display_image.save("display.png")