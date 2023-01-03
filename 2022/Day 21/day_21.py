from __future__ import annotations
from operator import mul, add, sub, floordiv
from collections import namedtuple

# Note: This Python solution is intended for testing the logic
#       that I am going to implement in C.

Monkey = namedtuple("Monkey", ("name", "number", "operand1", "operator", "operand2"))
monkeys:dict[str, Monkey] = {}

with open("input.txt", "rt") as file:
    for line in file:
        mk_name, mk_value = line.split(":")
        
        if (mk_value.count(" ") == 1):
            # If the value is a number, cast the string to integer
            monkey = Monkey(mk_name, int(mk_value), None, None, None)
        
        else:
            # If the value is an operation, split the string into operands and operator
            mk_val1, mk_opt, mk_val2 = mk_value.strip().split()

            # Replace the operator's character by the function that performs the operation
            match mk_opt:
                case "+":
                    mk_opt = add
                
                case "-":
                    mk_opt = sub
                
                case "*":
                    mk_opt = mul
                
                case "/":
                    mk_opt = floordiv
            
            assert callable(mk_opt)
            monkey = Monkey(mk_name, None, mk_val1, mk_opt, mk_val2)
        
        monkeys[mk_name] = monkey

def get_monkey_number(monkey_name:str) -> int:
    "Recursively calculate the number that a monkey is going to yell."
    
    monkey = monkeys[monkey_name]
    
    if monkey.number is not None:
        # If the monkey performs no operation, just return its number
        return monkey.number
    else:
        # Perform the monkey's opration on the operands (the other two monkeys' numbers)
        return monkey.operator(
            get_monkey_number(monkey.operand1),
            get_monkey_number(monkey.operand2)
        )

print("Part 1:", get_monkey_number("root"))