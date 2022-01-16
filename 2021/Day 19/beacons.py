from __future__ import annotations
from re import I
import numpy as np
from math import radians, sin, cos
from itertools import combinations, permutations
from dataclasses import dataclass

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

# Functions to perform rotations along all 3 axes
rotation = {
    "x": Rx,
    "y": Ry,
    "z": Rz,
}

# How to get each of the 6 faces to be facing up (↑)
"""Assuming this starting orientation:
  +y 
   |__+x
  /
+z
"""
face = {
    "+x": Rz(-90),  # 90° counterclockwise around the z-axis
    "+y": Ry(0),    # Identity operation
    "+z": Rx(90),   # 90° clockwise around the x-axis
    "-x": Rz(90),   # 90° clockwise around the z-axis
    "-y": Rx(180),  # 180° around the x-axis (around the z-axis would also achieve the same result) 
    "-z": Rx(-90)   # 90° counterclockwise around the x-axis
}

# Get the matrices for all 24 possible orientations of a cube in a 3D space
orientation:list[np.ndarray[3,3]] = []

# Loop through all 6 faces
for axis, starting_position in face.items():
    
    # Get the function to rotate around the face's axis
    axis_rotation = rotation[axis[1]]

    # Rotate each face by all 4 possible angles
    for deg in angle:
        # The '@' operator does matrix multiplication
        orientation.append(axis_rotation(deg) @ starting_position)

# Convert the matrices' elements to integers
# (because in our case all elements can only be either -1, 0, 1)
for i in range(len(orientation)):
    orientation[i] = np.round(orientation[i]).astype("int64")

# Parse the input

class ScannerData():
    """Hold the coordinates of the beacons seen by the scanner, as well the
    rotation and offset of the scanner in relation to a reference point."""

    def __init__(
        self,
        beacon: list[np.ndarray[1,3]] = None,   # A list of column matrices of (x,y,x) coordinates
        rotation: int = 4,                      # The index of the transformation matrix on the 'oriantation' list
        offset: np.ndarray[1,3] = None          # A column matrix of the (x,y,z) coordinate in relation to a reference point
    ) -> None:
        pass
    
        self.beacon = [] if (beacon is None) else beacon
        self.rotation = rotation
        self.offset = np.array([[0], [0], [0]], dtype="int64") if (offset is None) else offset
    
    def __repr__(self) -> str:
        return f"---Scanner---\nbeacons: {len(self.beacon)}\noffset:\n{self.offset}\nrotation matrix:\n{orientation[self.rotation]}\n"
    
    def get_coordinate(self, beacon_id:int) -> np.ndarray[1,3]:
        """Get the coordinate of a beacon corrected with the offset and the rotation."""
        return orientation[self.rotation] @ (self.beacon[beacon_id] + self.offset)

# Dictionary to hold each scanner and their respective data
scanner:dict[int, ScannerData] = {}

# Read the file
with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 19\input.txt", "rt") as file:
    raw_scanners = file.read().split("\n\n")

# Store the scanner file data from the file
for scan in raw_scanners:
    lines = scan.strip().split("\n")
    scanner_id = int(lines[0].split()[2])   # Number of the scanner
    scanner[scanner_id] = ScannerData()     # Structure to store the beacons' coordinates
    
    for line in lines[1:]:
        # Store the (x, y, z) coordinates as a column matrix
        x, y, z = line.split(",")
        scanner[scanner_id].beacon.append(
            np.array(
                [[int(x)], [int(y)], [int(z)]],
                dtype="int64"
            )
        )

# Part 1

