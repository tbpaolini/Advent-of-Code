from __future__ import annotations
from itertools import product
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
            [-sin(rad), 0, cos(rad)]
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
    "-y": Rx(180),  # 180° around the x-axis (around the z-axis would also achieve -y face up) 
    "-z": Rx(-90)   # 90° counterclockwise around the x-axis
}

# Get the matrices for all 24 possible orientations of a cube in a 3D space
orientation:list[np.ndarray] = []

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

    def __init__(self) -> None:
    
        # A list of column matrices of (x,y,x) coordinates in different orientations
        self.beacon: list[list[np.ndarray]] = []

        # The index of the transformation matrix on the 'oriantation' list
        self.rotation = 4

        # A column matrix of the (x,y,z) coordinate in relation to a reference point
        self.offset: np.ndarray = np.array([[0], [0], [0]], dtype="int64")

        # Which other scanners the current one is seeing
        self.next: ScannerData = None       # Next scanner
        self.previous: ScannerData = None   # Previous scanner

        self.cache = {}
    
    def __repr__(self) -> str:
        return f"---Scanner---\nbeacons: {len(self.beacon)}\noffset:\n{self.offset}\nrotation matrix:\n{orientation[self.rotation]}\n"
    
    def add_beacon(self, coordinate:np.ndarray) -> None:
        beacon_rotations = []
        for rotation_matrix in orientation:
            beacon_rotations.append(rotation_matrix @ coordinate)
        self.beacon.append(beacon_rotations)
    
    def get_coordinate(self, beacon_id:int) -> np.ndarray[np.uint64]:
        """Get the coordinate of a beacon corrected with the offset and the rotation."""
        
        fingerprint = (id(self.beacon[beacon_id]), id(orientation[self.rotation]), id(self.offset))
        
        if fingerprint in self.cache:
            return self.cache[fingerprint]
        else:
            coord = self.beacon[beacon_id][self.rotation] + self.offset
            self.cache[fingerprint] = coord
            return coord
    
    def get_all_coordinates(self) -> list[np.ndarray]:
        """Get the list of the coordinates of all beacons corrected with the offset and the rotation."""
        
        fingerprint = (id(orientation[self.rotation]), id(self.offset))

        if fingerprint in self.cache:
            return self.cache[fingerprint]
        else:
            coords = [self.get_coordinate(i) for i in range(len(self.beacon))]
            self.cache[fingerprint] = coords
            return coords
    
    def __getitem__(self, index:int) -> np.ndarray:
        return self.get_coordinate(index)
    
    def __iter__(self):
        self.__index = 0
        return self
    
    def __next__(self):
        if self.__index >= len(self.beacon): raise StopIteration
        result = self[self.__index]
        self.__index += 1
        return result
    
# Dictionary to hold each scanner and their respective data
scanners:dict[int, ScannerData] = {}

# Read the file
with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 19\test_input.txt", "rt") as file:
    raw_scanners = file.read().split("\n\n")

# Store the scanner file data from the file
for scan in raw_scanners:
    lines = scan.strip().split("\n")
    scanner_id = int(lines[0].split()[2])   # Number of the scanner
    scanners[scanner_id] = ScannerData()     # Structure to store the beacons' coordinates
    
    for line in lines[1:]:
        # Store the (x, y, z) coordinates as a column matrix
        x, y, z = line.split(",")
        scanners[scanner_id].add_beacon(
            np.array(
                [[int(x)], [int(y)], [int(z)]],
                dtype="int64"
            )
        )

# Part 1

def align_scanner(target_scanner:ScannerData, reference_scanner:ScannerData) -> bool:
    
    # Loop through all 24 possible scanner orientations
    for rotation in range(24):
        target_scanner.rotation = rotation
        
        # Try to align each beacon on the target with each beacon on the reference
        for target_beacon, reference_beacon in product(target_scanner, reference_scanner):
            target_scanner.offset = reference_beacon - target_beacon
            target_coordinates = target_scanner.get_all_coordinates()
            reference_coordinates = reference_scanner.get_all_coordinates()

            # Check which beacons overlap between target and reference
            aligned_beacons = 0
            total_beacons = len(target_coordinates)
            for count, tgt_coord in enumerate(target_coordinates):
                
                # If there is not enough beacons left to meet the 12 aligned beacons requirement
                if total_beacons - count < 12: break
                
                # Check if the current beacon has the same coordinate as a beacon on the reference
                if any(np.array_equal(tgt_coord, ref_coord) for ref_coord in reference_coordinates):
                    aligned_beacons += 1
                
                # If at least 12 beacons have been aligned
                if aligned_beacons >= 12: return True
            
            # Clear the coordinates cache, as the wrong results will not be necessary
            target_scanner.cache.clear()
    
    # When the scanners have not enough beacons in common
    return False

align_scanner(scanners[1], scanners[0])