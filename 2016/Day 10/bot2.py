from __future__ import annotations

class Bot():
    def __init__(self) -> None:
        self.chips:set[int] = {}
        self.low:int = None
        self.high:int = None
        self.low_destination:Bot|Output = None
        self.high_destination:Bot|Output = None
    
    def get_chip(self, value:int):
        self.chips.add(value)
        if len(self.chips) == 2:
            self.low, self.high = sorted(self.chips)
    
    def give_chip(self, other:Bot, value:int):
        assert value in self.chips
        other.get_chip(value)
        self.chips.remove(value)
    
    def set_command(self, low_destination:Bot|Output, high_destination:Bot|Output):
        self.low_destination = low_destination
        self.high_destination = high_destination

class Output():
    def __init__(self) -> None:
        self.bin:dict[int,set[int]] = {}
    
    def __getitem__(self, key):
        return self.bin[key]

bot_army:dict[int,Bot] = {}
output_bin = Output()

with open("input.txt") as file:
    raw_commands = [tuple(line.split()) for line in file.readlines()]

for command in raw_commands:
    if command[0] == "value":
        pass
    elif command[1] == "bot":
        pass