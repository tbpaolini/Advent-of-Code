import numpy as np
from itertools import product
from math import prod

with open("input.txt", "rt") as file:
    values_list = []
    
    for line in file:
        values_list += [[int(char) for char in line.rstrip()]]
    
    height_map = np.array(values_list)

# Pad the map with the value 10 on each corner
h_map = np.pad(height_map, 1, mode="constant", constant_values=10)
x_len, y_len = height_map.shape

# Part 1

risk_level = 0
low_points = []
for x, y in product(range(1, x_len+1), range(1, y_len+1)):
    centre = h_map[y, x]
    north  = h_map[y-1, x]
    south  = h_map[y+1, x]
    west   = h_map[y, x-1]
    east   = h_map[y, x+1]

    # Whether all 4 points around the centre are bigger
    region = np.array([north, south, west, east])
    if np.all(centre < region):
        risk_level += 1 + centre
        low_points += [(y, x)]

print(f"Part 1: {risk_level}")

# Part 2

# Get the size of a basin of a low point
def get_basin_size(y, x):
    global h_map
    basin = {(y, x)}    # All the (y, x) points in a basin
    history = set()     # The (y, x) points that were searched
    
    # Recursively search through the area
    def search_basin(y, x):
        nonlocal basin, history
        if (y, x) in history: return    # Exit if the point has been already searched

        # Other points to search
        to_search = set()

        # The four directions to search on
        directions = ((1, 0), (-1, 0), (0, 1), (0, -1))

        for y_dir, x_dir in directions:
        
            y_pos = y + y_dir
            x_pos = x + x_dir
            point_a = h_map[y, x]           # Starting point
            point_b = h_map[y_pos, x_pos]   # Neighboring point
            
            # If the next point bigger than the previous point and less than 9
            while (point_b > point_a) and (point_b < 9):
                
                coord = (y_pos, x_pos)
                basin.add(coord)
                if coord not in history: to_search.add(coord)

                # Move to the next point
                point_a = h_map[y_pos, x_pos]
                y_pos += y_dir
                x_pos += x_dir
                point_b = h_map[y_pos, x_pos]
            
            # Update the search history
            history.add((y, x))

            # Search the new points
            for point in to_search:
                search_basin(*point)
    
    # Begin the recursive search
    search_basin(y, x)
    return len(basin)

# Find the size of all basins
basin_sizes = []
for basin in low_points:
    basin_sizes += [get_basin_size(*basin)]

# Get the 3 largest basins and multiply their sizes
top_3_basins = list(sorted(basin_sizes, reverse=True))[:3]
answer_part2 = prod(top_3_basins)

print(f"Part 2: {answer_part2}")