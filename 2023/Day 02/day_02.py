from math import prod

games = []

# Each game is broken into a list of dictionaries with the amounts of each cube
with open("input.txt", "rt") as file:
    for line in file:
        # Get each draw from the game
        game = line.strip().split(": ")[1].split("; ")
        for i in range(len(game)):
            game[i] = game[i].split(", ")
            draw = game[i]
            
            # Get each cube from the draw
            my_dict = {}
            for cube in draw:
                count, color = cube.split(" ")
                my_dict[color] = int(count)
            game[i] = my_dict
        
        games.append(game)

# Maximum counts for Part 1
max_cube = {
    "red": 12,
    "green": 13,
    "blue": 14,
}

#---------- Part 1 ----------#

solution_p1 = 0
for n, game in enumerate(games, start=1):
    valid = True
    
    for draw in game:
        for color, count in draw.items():
            if count > max_cube[color]:
                # If a color count is greater than its maximum, then the game is invalid
                valid = False
                break
        if not valid: break
    
    if valid:
        solution_p1 += n

print("Part 1:", solution_p1)

#---------- Part 2 ----------#

solution_p2 = 0
for game in games:
    min_cube = {
    "red": 0,
    "green": 0,
    "blue": 0,
    }

    for draw in game:
        for color, count in draw.items():
            if (min_cube[color] < count):
                # The minimum amount of a color should be the greatest amount of that color among all draws
                min_cube[color] = count
    
    solution_p2 += prod(min_cube.values())

print("Part 2:", solution_p2)
