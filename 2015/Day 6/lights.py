import numpy as np
import re

with open("input.txt", "rt") as file:
    instructions_list = [line.strip() for line in file]

# Part 1

class Grid():
    
    regex = re.compile(r"(turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+)")
    
    def __init__(self) -> None:
        self.lights = np.zeros(shape=(1000, 1000), dtype="bool")
    
    def turn_on(self, x1, y1, x2, y2):
        self.lights[x1:x2+1, y1:y2+1] = True
    
    def turn_off(self, x1, y1, x2, y2):
        self.lights[x1:x2+1, y1:y2+1] = False
    
    def toggle(self, x1, y1, x2, y2):
        self.lights[x1:x2+1, y1:y2+1] = np.logical_not(self.lights[x1:x2+1, y1:y2+1])
    
    def instruct(self, instruction):
        result = self.regex.search(instruction).groups()
        command = result[0]
        x1, y1, x2, y2 = (int(value) for value in result[1:])

        if command == "turn on":
            self.turn_on(x1, y1, x2, y2)
        elif command == "turn off":
            self.turn_off(x1, y1, x2, y2)
        elif command == "toggle":
            self.toggle(x1, y1, x2, y2)

christimas_lights = Grid()

for instruction in instructions_list:
    christimas_lights.instruct(instruction)

lights_count = np.count_nonzero(christimas_lights.lights)
print(lights_count)

# Part 2

class AnalogGrid(Grid):
    def __init__(self) -> None:
        self.lights = np.zeros(shape=(1000, 1000), dtype="int64")
    
    def turn_on(self, x1, y1, x2, y2):
        self.lights[x1:x2+1, y1:y2+1] += 1
    
    def turn_off(self, x1, y1, x2, y2):
        pattern = self.lights[x1:x2+1, y1:y2+1] - 1
        np.clip(pattern, 0, None, out=pattern)
        self.lights[x1:x2+1, y1:y2+1] = pattern
    
    def toggle(self, x1, y1, x2, y2):
        self.lights[x1:x2+1, y1:y2+1] += 2

christimas_lights = AnalogGrid()

for instruction in instructions_list:
    christimas_lights.instruct(instruction)

total_brightness = np.sum(christimas_lights.lights)
print(total_brightness)