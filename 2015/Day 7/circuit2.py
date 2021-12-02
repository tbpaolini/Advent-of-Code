import numpy as np
import operator
from collections import namedtuple, deque

operations = {
    "AND": operator.and_,
    "OR": operator.or_,
    "RSHIFT": operator.rshift,
    "LSHIFT": operator.lshift,
    "NOT": operator.invert,
}

Instruction = namedtuple("Instruction", "value1 oper value2")

commands_list = []
with open("input2.txt", "rt") as file:
    for line in file:
        inp, out = line.strip().split(" -> ")
        inp = inp.split()
        inp_len = len(inp)

        if inp_len == 1:
            value1 = np.uint16(inp[0]) if inp[0].isdecimal() else inp[0]
            cmd = None
            value2 = None
        
        elif inp_len == 2:
            value1 = None
            cmd = operations[inp[0]]
            value2 = np.uint16(inp[1]) if inp[1].isdecimal() else inp[1]
        
        elif inp_len == 3:
            value1 = np.uint16(inp[0]) if inp[0].isdecimal() else inp[0]
            cmd = operations[inp[1]]
            value2 = np.uint16(inp[2]) if inp[2].isdecimal() else inp[2]
        
        command = Instruction(value1, cmd, value2)
        commands_list.append((command, out))

# Part 2
wires = {}
my_commands = commands_list.copy()
completed = deque()

while my_commands:
    for index, ((value1, oper, value2), output) in enumerate(my_commands):
        
        if isinstance(value1, str): value1 = wires.get(value1)
        if isinstance(value2, str): value2 = wires.get(value2)

        if (value1 is None) and (value2 is None):
            continue
        
        elif (value1 is None) and (oper == operations["NOT"]) and (value2 is not None):
            wires[output] = wires.get(output, np.uint16(0)) + oper(value2)
        
        elif (value1 is not None) and (value2 is not None):
            wires[output] = wires.get(output, np.uint16(0)) + oper(value1, value2)
        
        elif (value1 is not None) and (oper is None) and (value2 is None):
            wires[output] = wires.get(output, np.uint16(0)) + value1
        
        else:
            continue

        completed.append(my_commands[index])
    
    while completed:
        command = completed.popleft()
        my_commands.remove(command)

print(wires["a"])