from __future__ import annotations
from math import ceil, floor, prod
from itertools import permutations

with open("input.txt", "rt") as file:
    homework = tuple(eval(line) for line in file)

class SnailfishValue():

    def __init__(self, value:int, pos:int, depth:int, path:list) -> None:
        self.value = value      # The integer value in a pair
        self.pos = pos          # The position of the value in the pair
        self.depth = depth      # How deep the pair is
        self.path = path        # The path traversed through the nested lists until the parent list of the value:
                                # list[a][b][c][d] = value --> path = [a, b, c]
    
    def __repr__(self) -> str:
        return f"SnailfishValue(value={self.value}, pos={self.pos}, depth={self.depth}, path={self.path})"

class SnailfishNumber():

    def __init__(self, number:list[list|int]) -> None:
        self.number = number                    # The nested lists that represent the snailfish number
        self.values:list[SnailfishValue] = []   # List of each integer value and its attributes
        self.refresh_values()                   # Populate the above list
        self.reduce()                           # Perform reduction according to the puzzle's rules
    
    def refresh_values(self, pair:list[list|int]=None, depth:int=0, path:list=[]) -> None:
        
        # When at the root node of the shellfish number
        if pair is None: pair = self.number     # The input is the whole shellfish number itself
        if depth == 0: self.values.clear()      # Clear the list of values

        # Recursively navigate through the shellfish number
        for pos, value in enumerate(pair):
            if isinstance(value, list):
                # When the element is another list, call the function on that list
                self.refresh_values(value, depth+1, path+[pos])
            else:
                # When the element is an integer, add it and its attributes to the values list
                self.values += [SnailfishValue(value, pos, depth, path.copy())]
    
    def update_number(self):
        """Rebuild the nested list that represents the snailfish number."""
        
        # Empty binary tree of depth 4
        root = [[[[[], []], [[], []]], [[[], []], [[], []]]], [[[[], []], [[], []]], [[[], []], [[], []]]]]
        
        # Navigate through the tree and insert the values
        for integer in self.values:
            tree = root                                     # Start from the root node
            for node in integer.path: tree = tree[node]     # Go down a depth following the value's path
            tree[integer.pos] = integer.value               # Insert value on its position in the node
        
        # Replace the old nested list by the new one
        self.number = root
    
    def __len__(self) -> int:
        """Amount of integer values in the shellfish number."""
        return len(self.values)
    
    def __repr__(self) -> str:
        """Constructor of the shellfish number."""
        return f"SnailfishNumber({self.number})"
    
    def __add__(self, other:SnailfishNumber) -> SnailfishNumber:
        """Addition of two snailfish numbers: number_a + number_b"""
        if not isinstance(other, SnailfishNumber):
            raise TypeError("Unfortunately snailfishes do not know how to add human numbers.")
        
        # Concatenate both numbers and reduce the result if necessary
        result = SnailfishNumber([self.number] + [other.number])
        result.reduce()
        return result
    
    def __iadd__(self, other:SnailfishNumber) -> SnailfishNumber:
        """In-place addition of snailfish numbers: number_a += number_b"""
        if not isinstance(other, SnailfishNumber):
            raise TypeError("Unfortunately snailfishes do not know how to add human numbers.")
        
        # Amount of values before the addition
        self_len = len(self)

        # Concatenate the numbers
        self.values += other.values
        self.number = [self.number] + [other.number]

        # Update the attributes of the values
        for value in self.values[:self_len]: value.path.insert(0, 0)    # Prepend 0 to the path of the old values
        for value in self.values[self_len:]: value.path.insert(0, 1)    # Prepend 1 to the path of the new values
        for value in self.values: value.depth += 1                      # Increase by 1 the depth of all values
        
        # Perform reduction if necessary
        self.reduce()
        return self
    
    def reduce(self):

        # Main loop of the reduction process
        while True:
            max_index = len(self) - 1

            # Check pairs for explosions
            exploded = False
            for index in range(max_index):
                number_1 = self.values[index]
                number_2 = self.values[index+1]
                if (number_1.path != number_2.path): continue
                
                # If the pair is on depth 4 and above
                if (number_1.depth >= 4):

                    # Explode!
                    neighbor_1 = neighbor_2 = None
                    if index > 0:
                        # Add the left value to the left neighbor
                        neighbor_1 = self.values[index-1]
                        neighbor_1.value += number_1.value
                    if index < max_index-1:
                        # Add the right value to the right neighbor
                        neighbor_2 = self.values[index+2]
                        neighbor_2.value += number_2.value
                    
                    # Get the path to the exploded pair
                    new_pos = number_1.path[-1]
                    new_depth = number_1.depth - 1
                    new_path = number_1.path[:-1]

                    # Replace the exploded pair by a zero
                    self.values[index] = SnailfishValue(0, new_pos, new_depth, new_path)
                    del self.values[index+1]

                    exploded = True
                    break
                
            # Restart the reducing loop if an explode happened
            if exploded: continue

            # Check values for splits
            splitted = False
            # for index, integer in enumerate(self.values):
            for index in range(len(self)):
                integer = self.values[index]
                if integer.value >= 10:
                    
                    # Calculate the values of the new pair
                    new_int_1 = floor(integer.value / 2)
                    new_int_2 = ceil(integer.value / 2)

                    # Position of each value in the new pair
                    new_pos_1 = 0
                    new_pos_2 = 1

                    # Calculate the path of the new pair
                    my_depth = integer.depth + 1
                    new_path = integer.path + [integer.pos]

                    # Add the new pair to the shellfish number
                    new_value_1 = SnailfishValue(new_int_1, new_pos_1, my_depth, new_path)
                    new_value_2 = SnailfishValue(new_int_2, new_pos_2, my_depth, new_path)

                    # Rebuild the list of values of the shellfish number
                    self.values = self.values[:index] + [new_value_1, new_value_2] + self.values[index+1:]
                    splitted = True
                    break

            # Restart the reducing loop if a split happened
            if splitted: continue
            
            # Exit the reducing loop when there are no more explodes or splits
            break

        # Rebuild the representation of the snailfish number
        self.update_number()
    
    def magnitude(self) -> int:
        """Calculate the magnitude of the snailfish number.
        Values on position 0 have weight 3, and those on position 1 have weight 2."""

        magnitude = 0

        # The value is multiplied by 3 for each [0] in its path,
        # and by 2 for each [1] on the path
        for integer in self.values:
            full_path = integer.path + [integer.pos]
            multiplier = prod(3 if index==0 else 2 for index in full_path)
            magnitude += integer.value * multiplier
        
        return magnitude

if __name__ == "__main__":

    # Part 1
    
    result = SnailfishNumber(homework[0])
    for number in homework[1:]:
        result = result + SnailfishNumber(number)
    
    magnitude = result.magnitude()
    
    print(f"Part 1: {magnitude}")

    # Part 2

    max_magniture = 0

    for number_a, number_b in permutations(homework, 2):
        my_sum = SnailfishNumber(number_a) + SnailfishNumber(number_b)
        max_magniture = max(my_sum.magnitude(), max_magniture)
    
    print(f"Part 2: {max_magniture}")