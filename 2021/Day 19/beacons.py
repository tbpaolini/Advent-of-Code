from __future__ import annotations
import numpy as np
from math import radians, sin, cos

# 3D Rotation matrices
def Rx(deg:int) -> np.ndarray:
    """Rotate on the x-axis by an angle given in degrees."""
    
    # Convert the angle to radians and return the rotation matrix
    rad = radians(deg)
    return np.array(
        [
            [1,        0,         0],
            [0, cos(rad), -sin(rad)],
            [0, sin(rad),  cos(rad)]
        ]
    )

def Ry(deg:int) -> np.ndarray:
    """Rotate on the y-axis by an angle given in degrees."""
    
    # Convert the angle to radians and return the rotation matrix
    rad = radians(deg)
    return np.array(
        [
            [ cos(rad), 0, sin(rad)],
            [        0, 1,        0],
            [-sin(deg), 0, cos(rad)]
        ]
    )

def Rz(deg:int) -> np.ndarray:
    """Rotate on the z-axis by an angle given in degrees."""
    
    # Convert the angle to radians and return the rotation matrix
    rad = radians(deg)
    return np.array(
        [
            [cos(rad), -sin(rad), 0],
            [sin(rad),  cos(rad), 0],
            [       0,         0, 1]
        ]
    )

# Cube rotations

# Angles to rotate
angle = (0, 90, 180, 270)

# Rotations along all 3 axes
rotation = {
    "x": Rx,
    "y": Ry,
    "z": Rz,
}

# Inversion of the direction that a axis is facing
invert = {
    "x": Rz(180),
    "y": Rx(180),
    "z": Ry(180)
}

# Get the matrices for all 24 possible orientations of a cube in a 3D space
orientation:list[np.ndarray] = []

# Loop through the 4 angles
for deg in angle:

    # Loop through the 3 axes
    for axis, rot in rotation.items():
        
        # Rotate the axis by all 4 angles
        orientation.append(rot(deg))

        # Invert the axis then rotate by all 4 angles
        orientation.append(rot(deg) @ invert[axis])  # The '@' operator does matrix multiplication

# Convert the matrices' elements to integers
# (because in our case all elements can only be either -1, 0, 1)
for i in range(len(orientation)):
    orientation[i] = np.round(orientation[i]).astype("int64")

# Parse the input
scanner:dict[int, list[np.ndarray]] = {}
with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 19\input.txt", "rt") as file:
    raw_scanners = file.read().split("\n\n")

for scan in raw_scanners:
    lines = scan.strip().split("\n")
    scanner_id = int(lines[0].split()[2])   # Number of the scanner
    scanner[scanner_id] = []                # List to store the beacons' coordinates
    
    for line in lines[1:]:
        # Store the (x, y, z) coordinates as a column matrix
        x, y, z = line.split(",")
        scanner[scanner_id].append(
            np.array(
                [[int(x)], [int(y)], [int(z)]],
                dtype="int64"
            )
        )