import numpy as np

with open("input.txt", "rt") as file:
    grid = []
    for line in file:
        grid.append([int(char) for char in line.rstrip()])

grid = np.array(grid, dtype="uint32")       # Store the energy values
mask = np.zeros_like(grid, dtype="bool")    # Keep track of who blinked in the same step
max_y, max_x = grid.shape                   # Max dimensions of the grid
blink_count = 0                             # Count the blinks until step 100

def print_grid():
    text = ""
    for row in grid:
        text += " ".join(str(val).rjust(2) for val in row) + "\n"
    print(text)

sync = []   # Steps on which all blink at once
step = 0
while (step < 100) or (not sync):
    # Check if they blinked after previous step
    if np.all(grid == 0): sync += [step]
    
    # Increase all values by 1 and check who will blink
    mask[:] = False
    grid += 1
    blink = (grid > 9)

    # Keep processing the blinks until all have been processed
    while np.any(blink):
        row, column = np.where(blink)   # Coordinates of who blinked
        mask |= blink                   # Mark who blinked
        
        # Add 1 to each 3 x 3 area centered on the blink
        for y, x in zip(row, column):
            y1, x1 = max(y-1, 0), max(x-1, 0)
            y2, x2 = min(y+2, max_y+1), min(x+2, max_x+1)
            grid[y1:y2, x1:x2] += 1
            if step < 100: blink_count += 1
        
        # Check if there is someone else to blink
        blink = (grid > 9) & (~mask)
    
    # Set to 0 who blinked
    grid[mask] = 0
    step += 1

print(f"Part 1: {blink_count}")
print(f"Part 2: {sync[0]}")