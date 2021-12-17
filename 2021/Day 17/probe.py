import re
import numpy as np
from itertools import repeat
from typing import Tuple

# Parse the area limits from the input
with open("input.txt", "rt") as file:
    text_regex = re.compile(r"x=(-*\d+)\.\.(-*\d+), y=(-*\d+)\.\.(-*\d+)")
    raw_text = file.read()
    coordinates = text_regex.search(raw_text).groups()
    x_min, x_max, y_min, y_max = (int(coord) for coord in coordinates)

def point_collision(x, y, x1=x_min, x2=x_max, y1=y_min, y2=y_max) -> bool:
    "Whether a point is in the rectangle of vertices (x1, y1) (x2, y2)"

    # Check whether the point is between both vertices of the target area
    within_x = (x >= x1) and (x <= x2)
    within_y = (y >= y1) and (y <= y2)

    if within_x and within_y: return True
    else: return False

def trajectory(x_speed, y_speed, x=0, y=0, x_max=x_max, y_min=y_min) -> Tuple[Tuple[int,int]]:
    """Returns a list of the (x, y) coordinates in the trajectory."""
    
    x_coord = [x]
    y_coord = [y]

    # Move the probe until it goes beyond the target area
    while x <= x_max and y >= y_min:
        x += x_speed
        y += y_speed
        x_coord += [x]
        y_coord += [y]
        x_speed = max(0, x_speed-1)
        y_speed -= 1
    
    return (x_coord, y_coord)

def check_trajectory(x_speed, y_speed, x=0, y=0, x1=x_min, x2=x_max, y1=y_min, y2=y_max) -> bool:
    """Whether a trajectory hits the target area,"""

    # Get the (x, y) coordinates of all points in the trajectory
    x_path, y_path = trajectory(x_speed, y_speed, x, y, x2, y1)

    # Check if any of those points is inside the target area
    hit_target = any(
        map(
            point_collision,
            x_path,
            y_path,
            repeat(x1),
            repeat(x2),
            repeat(y1),
            repeat(y2)
        )
    )
    
    if hit_target:
        # Return the initial speeds if the trajectory hits the target area
        return (x_speed, y_speed)
    else:
        return None

# Get the speeds between x_speed [0, 250) and y_speed [-250, 250)
x_range = np.arange(0, 250)
y_range = np.arange(-250, 250)

# All combinations of (x_speed, y_speed) within the ranges
x_mesh, y_mesh = np.meshgrid(x_range, y_range)

# Test all those combinations if they hit
print("This might take around 10 seconds. Please be patient...")
launch_tests = map(check_trajectory, x_mesh.ravel(), y_mesh.ravel())
successes = [hit for hit in launch_tests if hit]

# Trajectory that got the highest y
max_x_speed, max_y_speed = max(successes, key = lambda f: f[1])
x_path, y_path = trajectory(max_x_speed, max_y_speed)
max_height = max(y_path)
print(f"Part 1: {max_height}")

# How many trajectories were successful
success_count = len(successes)
print(f"Part 2: {success_count}")