from typing import List, Tuple

with open("input.txt") as file:
    raw_lines = file.readlines()

molecule = raw_lines[-1].strip()
replacements = tuple(
    (key, value.strip()) for key, value in
    (line.split(" => ") for line in raw_lines[:-2])
)

# Part 1

unique = set()
for key, value in replacements:
    fragments = molecule.split(key)
    for i in range(1, len(fragments)):
        unique.add(
            key.join(fragments[:i]) + value + key.join(fragments[i:])
        )

print(f"Part 1: {len(unique)}")

# Part 2

from random import shuffle
replacements = list(replacements)

def try_synthesis():
    steps = 0
    index = 0
    mol = molecule
    repl_count = len(replacements)

    while index < repl_count:
        key, value = replacements[index]
        if value in mol:
            mol = mol.replace(value, key, 1)
            steps += 1
            if mol == "e":
                print(f"Part 2: {steps}")
                return True
            shuffle(replacements)
            index = 0
        else:
            index += 1

while not try_synthesis():
    pass

# ############################################################# #
# Bellow is my previous failed attempt at Part 2.               #
# I believe it would work if it was let to run for long enough, #
# but it was taking too long.                                   #
# ############################################################# #

# # Part 2

# def count_atoms(my_molecule:str) -> int:
#     return sum(char.isupper() for char in my_molecule)

# history = set()
# finished = False
# steps = 0
# def synthesis(end:str, start:str="e", count=0):
#     global replacements, history, finished, steps
#     print(f"{count}: {end}")

#     if finished: return

#     if end == start:
#         finished = True
#         # input(count)
#         steps = count
    
#     for value, key in sorted(replacements, key=lambda f: len(f[1]), reverse=True):  # <-- Reversing the (key, value) pair (we are going from the final product to the starting point)
#         if finished: break
#         fragments = end.split(key)
#         fragments_num = len(fragments)
#         mol_set = set()
        
#         if fragments_num > 1:
#             for i in range(1, fragments_num):
#                 mol = key.join(fragments[:i]) + value + key.join(fragments[i:])
#                 if mol not in history:
#                     mol_set.add(mol)
#                     history.add(mol)
        
#         for my_molecule in sorted(mol_set, key=len):
#             synthesis(my_molecule, count=count+1)
#             if finished: break
#             # if result is not None:
#             #     return result


# synthesis(molecule)
# print(steps)