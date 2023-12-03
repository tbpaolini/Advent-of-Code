from collections import Counter
games = []

with open("input.txt", "rt") as file:
    for line in file:
        game = line.strip().split(": ")[1].split("; ")
        for i in range(len(game)):
            game[i] = game[i].split(", ")
            draw = game[i]
            my_dict = {}
            for cube in draw:
                count, color = cube.split(" ")
                my_dict[color] = int(count)
            game[i] = my_dict
        games.append(game)

max_cube = {
    "red": 12,
    "green": 13,
    "blue": 14,
}

solution_p1 = 0
for n, game in enumerate(games, start=1):
    valid = True
    
    for draw in game:
        for color, count in draw.items():
            if count > max_cube[color]:
                valid = False
                break
        if not valid: break
    
    if valid:
        solution_p1 += n

print(solution_p1)