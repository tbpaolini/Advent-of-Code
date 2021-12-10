from collections import deque, namedtuple
from statistics import median

with open("input.txt", "rt") as file:
    lines = [line.rstrip() for line in file]

# Match each character with its counterpart and score
Char = namedtuple("Char", "match score")
opening = {
    "(": Char(")", 1),
    "[": Char(")", 2),
    "{": Char(")", 3),
    "<": Char(")", 4)
}
closing = {
    ")": Char("(", 3),
    "]": Char("[", 57),
    "}": Char("{", 1197),
    ">": Char("<", 25137)
}

markers = deque()        # Stack for the opening characters found (last in, first out)
error_score = 0          # Total score for the illegal characters
autocorrect_scores = []  # List for the autocorrect scores

# Analyse each line
for line in lines:
    
    markers.clear()     # Empty the stack
    corrupted = False   # If the line has a wrong closing character
    
    # Analyse each character on the line
    for char in line:
        if char in opening:
            # If it is an opening character, add it to the stack
            markers.append(char)
        
        else:
            # If it is a closing character, compare it
            # with the opening on the top of the stack
            old_char = markers.pop()
            if old_char != closing[char].match:
                # If the opening and closing does not match,
                # update the error score and mark line as corrupted
                error_score += closing[char].score
                corrupted = True
                break
    
    # Calculate the autocorrect score, if the line is not corrupted
    if not corrupted:
        score = 0
        while markers:
            old_char = markers.pop()
            score *= 5
            score += opening[old_char].score
        autocorrect_scores += [score]

# Get the median of the autocorrect scores
middle_score = median(autocorrect_scores)

print(f"Part 1: {error_score}")
print(f"Part 2: {middle_score}")