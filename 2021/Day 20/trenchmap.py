import numpy as np
from itertools import product
from pathlib import Path
from os import mkdir
from typing import Sequence

with open("input.txt") as file:
    
    # Read algorithm
    decoding_table = tuple(1 if char == "#" else 0 for char in file.readline().rstrip())
    file.readline()
    
    # Read image
    img_list = []
    for line in file: img_list.append([1 if char == "#" else 0 for char in line.rstrip()])
    img_array = np.array(img_list, dtype="bool")

def decode(img_array:np.ndarray, decoding_table:Sequence[bool], steps:int=1) -> np.ndarray:
    """Perform the decoding on a boolean image array a number of times specified in 'steps'."""
    
    # Pad the array with enough zeroes to take in account the "infinite size"
    # (For each step the image grows by 2 pixels, because the first neighboring
    #  pixels are also affected by their own neighbors)
    input_array = img_array.copy()
    input_array_pad = np.pad(input_array, pad_width=steps*2, mode="constant", constant_values=0)
    
    # Dimensions of the padded array
    height, width = input_array_pad.shape
    
    # Dimensions are shrinked by 1 so we don't go beyond the borders when checking a  3 x 3 area
    y_range = range(1, height-1)
    x_range = range(1, width-1)
    
    # Vector for converting a binary vector into a decimal scalar
    bin_to_dec = np.fromiter((2**n for n in range(8, -1, -1)), dtype="uint16")

    # Iterate the algorithm for the specified number of steps
    for step in range(steps):
        
        # Array to save the output
        output_array = np.zeros_like(input_array_pad)

        # Loop through all 3 x 3 regions in the image
        for y, x in product(y_range, x_range):
            key = input_array_pad[y-1 : y+2, x-1 : x+2].ravel() @ bin_to_dec    # Dot product to convert binary to decimal
            value = decoding_table[key]     # Decoded value
            output_array[y-1, x-1] = value
        
        # Copy the output array to the input for the next step
        input_array_pad = output_array
    
    # Slice away the padded area from the image
    return output_array[-height:-steps*2, -width:-steps*2]

def save_image(image:np.ndarray, file_name:str, scale:int=5) -> None:
    """Save the image to the 'images' subdirectory, scaled to a specified factor."""
    
    # Get the current directory and create the 'images' subdirectory if needed
    img_dir = Path(__file__).parent / Path("images")
    if not img_dir.exists(): mkdir(img_dir)
    
    # Rescale the image
    height, width = image.shape
    height *= scale
    width *= scale
    
    # Write the file to disk
    img = Image.fromarray(image).resize((height, width), resample=Image.NEAREST)
    img.save(Path(img_dir, file_name).with_suffix(".png"))

# Part 1
output_part1 = decode(img_array, decoding_table, steps=2)
print(f"Part 1: {np.count_nonzero(output_part1)}")

# Part 2
output_part2 = decode(img_array, decoding_table, steps=50)
print(f"Part 2: {np.count_nonzero(output_part2)}")

# Try saving the images
try:
    from PIL import Image
    save_image(img_array, "input.png")
    save_image(output_part1, "output - part 1.png")
    save_image(output_part2, "output - part 2.png")
    print("The images were saved to the 'images' subdirectory.")
except ModuleNotFoundError:
    print("Please install the Pillow module in order to save the images.")
    pass