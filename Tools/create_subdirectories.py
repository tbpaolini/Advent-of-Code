from os import makedirs

# This script needs to be run from the directory where to create the subdirectories

for year in range(2015, 2021):
    for day in range(1, 26):
        try:
            makedirs(f"{year}\Day {day}")
        except FileExistsError:
            continue