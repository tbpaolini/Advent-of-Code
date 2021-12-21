from itertools import cycle, count, product
from collections import Counter
from typing import Tuple
from functools import lru_cache
from time import perf_counter

start_time = perf_counter()

# Parse the file
with open("input.txt", "rt") as file:
    initial_position = {}
    for player, line in enumerate(file, start=1):
        # 1 is being subtracted so we can safely take the modulo 10 of the position
        # (therefore 1 is going to be added to the score for each move)
        initial_position[player] = int(line.split()[-1]) - 1

# Part 1

def deterministic_dice(sides:int, count:int) -> int:
    # Repeate indefinitely each side in sequence
    sides = cycle(range(1, sides+1))

    # Yield the sum of 3 consecutive sides each function call
    while True: yield sum(next(sides) for _ in range(count))

# Initial game state
position = initial_position.copy()  # Position of each player
score = Counter()                   # Counter of their scores
d100 = deterministic_dice(100, 3)   # Dice roll generator
players = cycle(position)           # Alternate indefinitely between players
dice_count = count(3, 3)            # Infinite generator of integers that  keeps incrementing by 3

# Simulate the game
for player, roll, dice_count in zip(players, d100, dice_count):
    position[player] = (position[player] + roll) % 10   # Move the player through the board
    score.update({player: position[player] + 1})        # Increment their score
    if score[player] >= 1000: break                     # Stops when a player reaches 1000 points

min_score = min(score.values())
print(f"Part 1: {min_score * dice_count}")

# Part 2

# All combinations of 3 dices that yields the same sum
# (counts how many times a specific sum repeats among all possible rolls)
dice_sums = Counter(sum(rolls) for rolls in product((1,2,3), repeat=3))

@lru_cache(maxsize=None)    # <-- Cache the results of the function
                            # (when the function is called with the same parameters, it returns the same result without being executed)
def dirac_dice(p1_pos:int, p2_pos:int, p1_score:int=0, p2_score:int=0) -> Tuple[int, int]:
    global dice_sums
    
    # Return the win count when a player reaches 21 points
    if p1_score >= 21: return (1, 0)
    if p2_score >= 21: return (0, 1)

    # Cumulative amount of wins for each player
    p1_wins_total = 0
    p2_wins_total = 0

    # Iterate over all possible rolls in a turn
    for roll, amount in dice_sums.items():
        
        # Move the player and increment their score
        p1_pos_new = (p1_pos + roll) % 10
        p1_score_new = p1_score + p1_pos_new + 1

        # Pass the turn to the other player
        p2_wins, p1_wins = dirac_dice(p2_pos, p1_pos_new, p2_score, p1_score_new)

        # Increment the win counter by the results multiplied by the amount of times the roll repeats
        p1_wins_total += p1_wins * amount
        p2_wins_total += p2_wins * amount
    
    # Returns the final tally of victories
    return (p1_wins_total, p2_wins_total)

max_victories = max(
    dirac_dice(
        p1_pos = position[1],
        p2_pos = position[2]
    )
)

end_time = perf_counter()
print(f"Part 2: {max_victories}")
print(f"Time: {end_time - start_time:.3f} s")

# Credits for the idea for the solution of Part 2:
# https://www.reddit.com/r/adventofcode/comments/rl6p8y/comment/hpgdx9o/
# https://github.com/SwampThingTom/AoC2021/blob/main/Python/21-DiracDice/DiracDice.py