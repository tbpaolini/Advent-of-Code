from itertools import combinations_with_replacement

with open("input.txt", "rt") as file:
    ingredients = {}
    for line in file:
        name, raw_stats = line.split(": ")
        stats = (text.split() for text in raw_stats.split(", "))
        stats_dict = {stat: int(value) for stat, value in stats}
        ingredients.update({name: stats_dict})

scores = []
scores_500 = []
for count, recipe in enumerate(combinations_with_replacement(ingredients, 100)):
    capacity = 0
    durability = 0
    flavor = 0
    texture = 0
    calories = 0
    
    for item in recipe:
        stats = ingredients[item]
        capacity += stats["capacity"]
        durability += stats["durability"]
        flavor += stats["flavor"]
        texture += stats["texture"]
        calories += stats["calories"]
    
    my_score = max(capacity, 0) * max(durability, 0) * max(flavor, 0) * max(texture, 0)

    scores.append(my_score)
    if calories == 500:
        scores_500.append(my_score)

    print(f"Progress: {count+1} / 176851 ({(count+1) * 100 / 176851:.2f})%", end="\r")

print()
print(f"Part 1: {max(scores)}")
print(f"Part 2: {max(scores_500)}")