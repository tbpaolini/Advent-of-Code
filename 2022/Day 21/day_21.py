from __future__ import annotations
from operator import mul, add, sub, floordiv
from collections import namedtuple

# Note: This Python solution is intended for testing the logic
#       that I am going to implement in C.

Monkey = namedtuple("Monkey", ("name", "number", "operand1", "operator", "operand2"))
monkeys:dict[str, Monkey] = {}

with open("/home/tiago/Documentos/Advent of Code/2022/Day 21/input.txt", "rt") as file:
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

previous_number = monkeys["humn"].number
monkeys["root"] = Monkey("root", None, monkeys["root"].operand1, sub, monkeys["root"].operand2)
target = get_monkey_number("root")
previous_error = abs(target)

my_number = 1
monkeys["humn"] = Monkey("humn", my_number, None, None, None)
target = get_monkey_number("root")
my_error = abs(target)

rate = 10

while (my_error > 0.1):
    
    try:
        gradient = (my_number - previous_number) / (my_error - previous_error)
    except ZeroDivisionError:
        gradient = 1
    
    previous_number = my_number
    previous_error = my_error

    my_number += rate * gradient
    monkeys["humn"] = Monkey("humn", my_number, None, None, None)
    target = get_monkey_number("root")
    my_error = abs(target)

print("Part 2:", my_number)