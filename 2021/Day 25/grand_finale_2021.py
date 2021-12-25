import numpy as np
from time import perf_counter

empty = 0
east = 1
south = 2
mapping = {
    ".": empty,
    ">": east,
    "v": south
}

with open("input.txt") as file:
    seafloor = []
    for line in file: seafloor.append([mapping[char] for char in line.rstrip()])
    seafloor = np.array(seafloor, dtype="uint8")

start_time = perf_counter()
step = 0
while True:
    step += 1

    ### East movement ###

    # Check the positions of the sea cucumbers
    all_cucumbers = (seafloor != empty)
    east_cucumbers = (seafloor == east)
    
    # Roll the array elements along the rows by one space to the right
    # (it simulates the tentative east movements)
    east_move = np.roll(east_cucumbers, axis=1, shift=1)
    
    # Check if the tentative destination is free
    east_destination = (east_move & ~all_cucumbers)
    moving_east = np.roll(east_destination, axis=1, shift=-1) 

    # Set to empty the spots of the cucumbers that can move
    seafloor[moving_east] = empty

    # Place on the new spots the east-facing cucumbers that can move
    seafloor[east_destination] = east

    ### South movement ###

    # Update the positions of the sea cucumbers
    all_cucumbers = (seafloor != empty)
    south_cucumbers = (seafloor == south)

    # Roll the array elements along the columns by one space to the bottom
    # (it simulates the tentative south movements)
    south_move = np.roll(south_cucumbers, axis=0, shift=1)
    
    # Check if the tentative destination is free
    south_destination = (south_move & ~all_cucumbers)
    moving_south = np.roll(south_destination, axis=0, shift=-1) 

    # Set to empty the spots of the cucumbers that can move
    seafloor[moving_south] = empty

    # Place on the new spots the south-facing cucumbers that can move
    seafloor[south_destination] = south

    # Count all movements
    move_count = np.sum(east_destination + south_destination)
    if move_count == 0: break

total_time = perf_counter() - start_time

print(f"Part 1: {step} (took {total_time:.3f} seconds)")
print("Merry Christmas!")