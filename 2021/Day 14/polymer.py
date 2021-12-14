from collections import Counter
from typing import List, Tuple

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 14\input.txt", "rt") as file:
    molecule = file.readline().rstrip()
    file.readline()
    
    rules = []
    for line in file:
        rule = tuple(line.rstrip().split(" -> "))
        rules += [rule]

pair_count = Counter({rule[0]: 0 for rule in rules})

def polymerization(molecule:str, rules:List[Tuple[str,str]], steps:int) -> str:
    """Insert new atoms between atom pairs according to the rules.
    The process is repeated for 'steps' times."""
    
    for _ in range(steps):
        
        replacements = {}
        for pair, insert in rules:
            index = molecule.find(pair)
            while index != -1:
                replacements[index] = insert
                index = molecule.find(pair, index+1)
        
        new_molecule = "".join(molecule[i] + replacements.get(i, "") for i in range(len(molecule)))
        
        molecule = new_molecule

        print(_)
    
    return molecule

# Part 1

polymer = polymerization(molecule, rules, steps=10)
atom_counts = Counter(polymer).most_common()
most_count = atom_counts[0][1]
least_count = atom_counts[-1][1]

print(f"Part 1: {most_count - least_count}")