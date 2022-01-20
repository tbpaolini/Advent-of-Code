from __future__ import annotations
from itertools import permutations, product
import numpy as np
from math import radians, sin, cos
import pickle
from pathlib import Path
from uuid import uuid4

scanners_file_path:Path = Path(__file__).parent / "scanners.pickle"

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

    scanners_set = set()

    def __new__(cls:type, uuid:int = None) -> ScannerData:
        
        # Create the instance
        self = super().__new__(cls)
        
        # Create an unique ID for the instance
        if uuid is None:
            self.uuid = uuid4().int
        else:
            self.uuid = uuid
        
        return self
        # Note: The UUID will be used as the hash value and for keeping track of which other scanners have been checked to be near.
        #       We have created it here because the .__hash__() method would yield an error if we tried to set the .uuid property
        #       during the .__init__() method.

    def __init__(self) -> None:

        self.__count = len(self.scanners_set)   # Order in which the instance was created
        self.scanners_set.add(self)             # Keep track of all created instances of this class
    
        # A list of column matrices of (x,y,x) coordinates in different orientations
        self.beacon: list[list[np.ndarray]] = []

        # The index of the transformation matrix on the 'oriantation' list
        self.rotation = 4

        # A column matrix of the (x,y,z) coordinate in relation to a reference point
        self.offset: np.ndarray = np.array([[0], [0], [0]], dtype="int64")

        # Which other scanners the current one is seeing
        self.nodes: dict[ScannerData: np.ndarray] = {}
        
        # Store the UUID of the scanners checked if they are being seen by the current one
        self.nodes_tried: set = set()

        # Cache for the results of the methods
        self.cache = {}
    
    def __getnewargs__(self) -> tuple:
        """Arguments to pass to the .__new__() method when unpickling the object."""
        return (self.uuid,)
        # Note: We need the UUID value to be the same between different script executions,
        #       because previously created instances can be loaded from a pickle file and
        #       that value is used as the hash of the instance.
    
    def __repr__(self) -> str:
        return f"Scanner {self.__count}"
    
    def __str__(self) -> str:
        return f"---Scanner {self.__count}---\nbeacons: {len(self.beacon)}\noffset:\n{self.offset}\nrotation matrix:\n{orientation[self.rotation]}\n"
    
    def add_beacon(self, coordinate:np.ndarray) -> None:
        beacon_rotations = []
        for rotation_matrix in orientation:
            beacon_rotations.append(rotation_matrix @ coordinate)
        self.beacon.append(beacon_rotations)
    
    def get_coordinate(self, beacon_id:int) -> tuple[int]:
        """Get the coordinate of a beacon corrected with the offset and the rotation."""
        
        fingerprint = hash(("tuple", id(self.beacon[beacon_id]), id(orientation[self.rotation]), id(self.offset)))
        
        if fingerprint in self.cache:
            return self.cache[fingerprint]
        else:
            coord = tuple(self[beacon_id].ravel())
            self.cache[fingerprint] = coord
            return coord
    
    def get_all_coordinates(self) -> set[tuple[int]]:
        """Get the list of the coordinates of all beacons corrected with the offset and the rotation."""
        
        fingerprint = ("set", id(orientation[self.rotation]), id(self.offset))

        if fingerprint in self.cache:
            return self.cache[fingerprint]
        else:
            coords = {self.get_coordinate(i) for i in range(len(self.beacon))}
            self.cache[fingerprint] = coords
            return coords
    
    def reset(self):
        """Returns the scanner to its original rotation and offset"""
        self.rotation = 4
        self.offset = np.array([[0], [0], [0]], dtype="int64")
    
    def __getitem__(self, index:int) -> np.ndarray:
        fingerprint = hash(("array", id(self.beacon[index]), id(orientation[self.rotation]), id(self.offset)))
        if fingerprint in self.cache:
            return self.cache[fingerprint]
        else:
            item = self.beacon[index][self.rotation] + self.offset 
            self.cache[fingerprint] = item
            return item
    
    def __iter__(self):
        self.__index = 0
        return self
    
    def __next__(self):
        if self.__index >= len(self.beacon): raise StopIteration
        result = self[self.__index]
        self.__index += 1
        return result
    
    def __hash__(self) -> int:
        return self.uuid    # We need the object to be have a hash value so we can use it as a dictionary key
    
# Dictionary to hold each scanner and their respective data
scanners:dict[int, ScannerData] = {}

# Read the file
if scanners_file_path.exists():
    # Load the file with the scanners alignments ('scanners.pickle'), if it exists
    with open(scanners_file_path, "rb") as scanners_file:
        scanners = pickle.load(scanners_file)

else:
    # Rebuild the scanners dictionary from scratch if there is no  'scanners.pickle' file
    with open("input.txt", "rt") as file:
        raw_scanners = file.read().split("\n\n")

    # Store the scanner file data from the file
    for scan in raw_scanners:
        lines = scan.strip().split("\n")
        scanner_id = int(lines[0].split()[2])   # Number of the scanner
        scanners[scanner_id] = ScannerData()    # Structure to store the beacons' coordinates
        
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

    old_rotation = target_scanner.rotation
    
    # Loop through all 24 possible scanner orientations
    for rotation in range(24):
        target_scanner.rotation = rotation
        
        # Try to align each beacon on the target with each beacon on the reference
        old_offset = target_scanner.offset.copy()
        for target_beacon, reference_beacon in product(target_scanner, reference_scanner):
            target_scanner.offset = reference_beacon - target_beacon
            target_coordinates = target_scanner.get_all_coordinates()
            reference_coordinates = reference_scanner.get_all_coordinates()

            # Check if at least 12 beacons overlap between target and reference
            overlap = target_coordinates & reference_coordinates
            if len(overlap) >= 12: return True
            
            # Clear the coordinates cache, as the wrong results will not be necessary
            target_scanner.cache.clear()
            target_scanner.offset[:] = old_offset[:]
    
    # When the scanners have not enough beacons in common
    target_scanner.rotation = old_rotation
    return False

max_steps = len(scanners) * (len(scanners) - 1)
step = 0
print("Building map of scanners...")
for target, reference in permutations(scanners.values(), 2):
    print(f"Progress: {step * 100 / max_steps:.2f}%", end="\r")
    step += 1
    
    # Check if the target has already been checked against
    # (useful when a unfinished results have been loaded from disk)
    if target in reference.nodes_tried: continue
    
    # Attempt to align the pair
    aligned = align_scanner(target, reference)
    if aligned:
        reference.nodes[target] = (target.offset , target.rotation)
    
    reference.nodes_tried.add(target)   # Mark the pair as "tried"
    target.reset()                      # Reset the position and orientation of the target
    
    # Backup to disk the partial result
    if aligned:
        with open(scanners_file_path, "wb") as scanners_file:
            pickle.dump(scanners, scanners_file)

print("Progress: 100%  ")