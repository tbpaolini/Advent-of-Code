## Advent of code: Day 2 of 2023

[Language: Python]

Parsing the input is a greater challenge than the solution's logic itself. What I ended up doing was to parse each game into a list of dictionaries. One dictionary for each draw, containing the counts for each color.

Then for **Part 1** I checked if any of the counts in a game was greater than the maximum. And for **Part 2** I checked the maximum count for each color in a game.

**My Solution:** [day_02.py](https://github.com/tbpaolini/Advent-of-Code/blob/master/2023/Day%2002/day_02.py)
