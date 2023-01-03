from __future__ import annotations
from operator import mul, add, sub, truediv
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
                    mk_opt = truediv
            
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

print("Part 1:", round(get_monkey_number("root")))

# Initial value of our number
previous_number = monkeys["humn"].number

# Now the "root" monkey subtracts the values
# (if the result is zero, then the values are equal)
monkeys["root"] = Monkey("root", None, monkeys["root"].operand1, sub, monkeys["root"].operand2)

# Calculate how far from zero the result is
# (that is our "error function")
target = get_monkey_number("root")
previous_error = abs(target)

# First guess for our number
my_number = 0
monkeys["humn"] = Monkey("humn", my_number, None, None, None)

# How far from zero the result is after the guess
target = get_monkey_number("root")
my_error = abs(target)

# Learning rate of the minimization algorithm (gradient descent)
rate = 0.1

# Keep searching for the minimum until the error is smaller than 0.1
while (my_error > 0.1):
    
    # Calculate the slope of the function between the previous two guesses
    try:
        gradient = (my_number - previous_number) // (my_error - previous_error)
    except ZeroDivisionError:
        gradient = 1 if my_error < previous_error else -1
    
    # Remember the results of the previous guess
    previous_number = my_number
    previous_error = my_error

    # Move the guessed value in the direction that the error function decreases
    my_number -= rate * my_error * gradient
    monkeys["humn"] = Monkey("humn", my_number, None, None, None)
    
    # Recalculate the error
    target = get_monkey_number("root")
    my_error = abs(target)

print("Part 2:", round(my_number))