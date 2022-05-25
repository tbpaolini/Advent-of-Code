from __future__ import annotations
from typing import Optional
from os import get_terminal_size

Instruction = list[tuple[str,str|int,Optional[str|int]]]

class Computer():

    def __init__(self, instructions:Instruction, part2:bool=False) -> None:
        self.a:int = 0
        self.b:int = 0
        self.c:int = 0 if not part2 else 1
        self.d:int = 0
        
        self.instructions = instructions.copy()
        self.instruction_counter:int = 0
        step = 0

        while(self.instruction_counter >= 0 and self.instruction_counter < len(instructions)):
            instruction = self.instructions[self.instruction_counter]
            old_count = self.instruction_counter

            if len(instruction) == 3:
                asm, x, y = instruction
                assert asm in ("cpy", "inc", "dec", "jnz")
                exec(f"self.{asm}(x,y)")
            
            elif len(instruction) == 2:
                asm, x = instruction
                assert asm in ("cpy", "inc", "dec", "jnz")
                exec(f"self.{asm}(x)")
            
            else:
                raise ValueError()
            
            
            if step % 1000 == 0:
                size = get_terminal_size().columns - 1
                print(f"a={self.a}, b={self.b}, c={self.c}, d={self.d}".ljust(size), end="\r")
            
            step +=1
            
            if old_count == self.instruction_counter:
                self.instruction_counter += 1
        
        size = get_terminal_size().columns - 1
        print(f"a={self.a}, b={self.b}, c={self.c}, d={self.d}".ljust(size), end="\r")

    
    def cpy(self, x:str|int, y:str):
        assert (len(y) == 1) and (y in "abcd")
        
        if isinstance(x, str):
            assert (len(x) == 1) and (x in "abcd")
            exec(f"self.{y} = self.{x}")
        
        elif isinstance(x, int):
            exec(f"self.{y} = {x}")
        
        else:
            raise TypeError()
    
    def inc(self, x:str):
        assert (len(x) == 1) and (x in "abcd")
        if not isinstance(x, str): raise TypeError()

        exec(f"self.{x} += 1")
    
    def dec(self, x:str):
        assert (len(x) == 1) and (x in "abcd")
        if not isinstance(x, str): raise TypeError()

        exec(f"self.{x} -= 1")
    
    def jnz(self, x:str|int, y:int):
        if not isinstance(x, (str, int)): raise TypeError()
        if not isinstance(y, int): raise TypeError()

        if isinstance(x, str):
            assert (len(x) == 1) and (x in "abcd")
            x_is_zero = eval(f"self.{x} == 0")
            if not x_is_zero: self.instruction_counter += y
        
        else:
            if x != 0: self.instruction_counter += y

with open("input.txt", "rt") as file:
    instructions:Instruction = []
    
    for line in file:
        raw = line.split()
        assert len(raw) in (2, 3)
        
        asm = raw[0]
        x = int(raw[1]) if raw[1][-1].isdecimal() else raw[1]
        
        if len(raw) == 3:
            y = int(raw[2]) if raw[2][-1].isdecimal() else raw[2]
            instructions.append((asm, x, y))
        else:
            instructions.append((asm, x))

print("Note: There is a shortcut, instead of executing every single instruction. But I am just proud of the interpreter I made :)\n")

p1_result = Computer(instructions)
print(f"\nPart 1: {p1_result.a}\n")

p2_result = Computer(instructions, part2=True)
print(f"\nPart 2: {p2_result.a}\n")

input("Pres ENTER to close...")