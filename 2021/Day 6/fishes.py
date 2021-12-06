import numpy as np

with open("input.txt", "rt") as file:
    counters = np.fromiter(
        (int(value) for value in file.readline().split(",")),
        dtype="int8"
    )

# Store the counts for each value of remaining time
fishes_counter = np.fromiter(
    (np.count_nonzero(counters == i) for i in range(7)),
    dtype = "uint64"
)

# Store the counts for the newborn fishes in their cooldown time
cooldown_counter = np.zeros(2, dtype="uint64")

# Simulate the fish population growth over time
def simulation(time:int):
    global fishes_counter, cooldown_counter
    fishes = fishes_counter.copy()
    cooldown = cooldown_counter.copy()

    for _ in range(time):
        fishes = np.roll(fishes, -1)        # Roll the array by one position to the left
        cooldown = np.roll(cooldown, -1)    # Roll the array by one position to the left
        new_fishes = fishes[-1]             # Count the new fishes
        cooldown_finished = cooldown[-1]    # Count how many fishes have finished their cooldown
        fishes[-1] += cooldown_finished     # Add the fishes that finished the cooldown
        cooldown[-1] = new_fishes           # Put the newborn fishes on the cooldown array
    
    return np.sum(fishes) + np.sum(cooldown)

print(f"Part 1: {simulation(80)}")
print(f"Part 2: {simulation(256)}")