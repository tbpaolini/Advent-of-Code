from typing import Any

class Computer():

    def __init__(self) -> None:
        self.instructions = []
        self.header = 0
        self.register = dict(a=0, b=0)
    
    def __getattr__(self, __name: str) -> Any:
        if __name in self.register:
            return self.register[__name]
        else:
            raise AttributeError(f"Attribute '{__name}' not found.")
    
    def __setattr__(self, __name: str, __value: Any) -> None:
        if __name in ("a", "b"):
            self.register.update({__name: __value})
        else:
            super().__setattr__(__name, __value)
    
    def load(self, path:str="input.txt") -> None:
        
        with open(path, "rt") as file:
            for line in file:
                code, param = line.rstrip().split(" ", 1)
                if "," in param:
                    param1, param2 = param.split(", ")
                    param2 = int(param2)
                    self.instructions += [(code, (param1, param2))]
                else:
                    if not param.isalpha(): param = int(param)
                    self.instructions += [(code, (param,))]
    
    def hlf(self, r:str) -> None:
        self.register[r] //= 2
        self.header += 1
    
    def tpl(self, r:str) -> None:
        self.register[r] *= 3
        self.header += 1
    
    def inc(self, r:str) -> None:
        self.register[r] += 1
        self.header += 1
    
    def jmp(self, offset:int) -> None:
        self.header += offset
    
    def jie(self, r:str, offset:int) -> None:
        if self.register[r] % 2 == 0: self.header += offset
        else: self.header += 1
    
    def jio(self, r:str, offset:int) -> None:
        if self.register[r] == 1: self.header += offset
        else: self.header += 1
    
    def get_instruction(self):
        return self.instructions[self.header]
    
    def run(self):
        while self.header < len(self.instructions):
            instruction = self.get_instruction()
            handler = eval(f"self.{instruction[0]}")
            handler(*instruction[1])


if __name__ == "__main__":
    part1_computer = Computer()
    part1_computer.load()
    part1_computer.run()
    print(f"Part 1: {part1_computer.b}")

    part2_computer = Computer()
    part2_computer.load()
    part2_computer.a = 1
    part2_computer.run()
    print(f"Part 2: {part2_computer.b}")