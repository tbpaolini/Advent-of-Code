# Puzzle 1

def present_area(length, width, height):
    surface = 2*length*width + 2*width*height + 2*height*length
    extra = min(length*width, width*height, height*length)
    return surface + extra


present_list = []
with open("input.txt") as file:
    for line in file:
        dimensions = tuple(int(value) for value in line.split("x"))
        present_list.append(dimensions)

total_area = 0
for present in present_list:
    total_area += present_area(*present)

print(total_area)

# Puzzle 2

def ribbon_length(length, width, height):
    side_a, side_b = sorted((length, width, height))[:2]
    perimeter = 2*side_a + 2*side_b
    volume = length * width * height
    return perimeter + volume

total_length = 0
for present in present_list:
    total_length += ribbon_length(*present)

print(total_length)