from collections import Counter
from typing import List, Tuple

with open("input.txt", "rt") as file:
    molecule = file.readline().rstrip()
    file.readline()
    
    rules = []
    for line in file:
        rule = tuple(line.rstrip().split(" -> "))
        rules += [rule]

def count_atoms(molecule:str, rules:List[Tuple[str,str]], steps:int) -> Counter:
    """Count how many of each atom there will be after a number of polymerization steps"""

    # Initial count of the atoms and pairs
    atom_count = Counter(molecule)
    pair_count = Counter()
    for i in range(len(molecule)-1): pair_count.update((molecule[i:i+2],))

    # Perform polymerization
    for _ in range(steps):

        # Pair count at the beginning of the step
        # (necessary because all substitutions of the step occur at the same time)
        pair_count_start = pair_count.copy()

        # Apply all substitution rules
        for old_pair, new_atom in rules:

            # Count the old pair on the molecule
            # When it is 0, there are no sybstitutions to make
            old_pair_count = pair_count_start[old_pair]
            if old_pair_count == 0: continue
            
            # Form the new pairs
            new_pair_1 = old_pair[0] + new_atom
            new_pair_2 = new_atom + old_pair[1]

            # Remove the old pairs from the counter
            # (this is not just being set to zero becasue this same
            # pair might be created again by another substitution)
            pair_count.subtract({old_pair: old_pair_count})

            # Add the new pairs and atoms in the same amount as the old pair
            atom_count.update({new_atom: old_pair_count})
            pair_count.update({new_pair_1: old_pair_count, new_pair_2: old_pair_count})
    
    # Return the count of all atoms
    return atom_count

# Part 1 (10 steps)

atom_counts_p1 = count_atoms(molecule, rules, steps=10).most_common()
most_count_p1 = atom_counts_p1[0][1]
least_count_p1 = atom_counts_p1[-1][1]
print(f"Part 1: {most_count_p1 - least_count_p1}")

# Part 2 (40 steps)

atom_counts_p2 = count_atoms(molecule, rules, steps=40).most_common()
most_count_p2 = atom_counts_p2[0][1]
least_count_p2 = atom_counts_p2[-1][1]
print(f"Part 2: {most_count_p2 - least_count_p2}")


# Below is my attempt for Part 1, that didn't work for Part 2
# I am keeping it here because I liked how I managed to concatenate the string correctly

def polymerization(molecule:str, rules:List[Tuple[str,str]], steps:int) -> str:
    """Insert new atoms between atom pairs according to the rules.
    The process is repeated for 'steps' times."""

    if steps > 10:
        print("Sorry. This isn't going to work for Part 2, because memory usage grows exponentially with the number of steps.")
    
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

# polymer = polymerization(molecule, rules, steps=10)
# atom_counts = Counter(polymer).most_common()
# most_count = atom_counts[0][1]
# least_count = atom_counts[-1][1]

# print(f"Part 1: {most_count - least_count}")