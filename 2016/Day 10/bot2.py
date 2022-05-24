from __future__ import annotations
from itertools import chain
from math import prod

class Bot():
    def __init__(self, id:int) -> None:
        self.id = id
        self.chips:set[int] = set()
        self.low:int = None
        self.high:int = None
        self.low_destination:Bot|Output = None
        self.high_destination:Bot|Output = None
        self.low_index:int = None
        self.high_index:int = None
    
    def __repr__(self) -> str:
        return repr(self.chips)
    
    def get_chip(self, value:int):
        self.chips.add(value)
        if len(self.chips) == 2:
            self.low, self.high = sorted(self.chips)
            if (self.low == 17) and (self.high == 61):
                print(f"Part 1: {self.id}")
            
            self.high_destination[self.high_index].add(self.high)
            self.chips.remove(self.high)
            self.low_destination[self.low_index].add(self.low)
            self.chips.remove(self.low)
    
    def add(self, value:int):
        self.get_chip(value)
    
    def give_chip(self, other:Bot, value:int):
        assert value in self.chips
        other.get_chip(value)
        self.chips.remove(value)
    
    def set_command(self, low_destination:Bot|Output, low_index:int, high_destination:Bot|Output, high_index:int):
        self.low_destination = low_destination
        self.low_index = low_index
        self.high_destination = high_destination
        self.high_index = high_index

class BotArmy():
    def __init__(self) -> None:
        self.army:dict[int,Bot] = {}
    
    def __getitem__(self, key:int):
        if key not in self.army:
            self.army[key] = Bot(key)
        return self.army[key]
    
    def __repr__(self) -> str:
        return f"Bots: {repr(self.army)}"

class Output():
    def __init__(self) -> None:
        self.bin:dict[int,set[int]] = {}
    
    def __getitem__(self, key:int):
        if key not in self.bin:
            self.bin[key] = set()
        return self.bin[key]
    
    def __repr__(self) -> str:
        return f"Output: {repr(self.bin)}"

bot_army:dict|BotArmy = BotArmy()
output_bin:set|Output = Output()

with open("input.txt") as file:
    raw_commands = [tuple(line.split()) for line in file.readlines()]

for command in raw_commands:
    if command[0] == "bot":
        bot_index = int(command[1])
        
        low_destination = output_bin if command[5] == "output" else bot_army
        low_index = int(command[6])
        
        high_destination = output_bin if command[10] == "output" else bot_army
        high_index = int(command[11])

        bot_army[bot_index].set_command(low_destination, low_index, high_destination, high_index)

for command in raw_commands:
    if command[0] == "value":
        chip = int(command[1])
        bot = int(command[-1])
        bot_army[bot].get_chip(chip)

part2 = prod(chain(output_bin[0], output_bin[1], output_bin[2]))
print(f"Part 2: {part2}")