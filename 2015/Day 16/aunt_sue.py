sues = {}
with open("input.txt", "rt") as file:
    for line in file:
        name, raw_stats = line.split(": ", 1)
        stats = [text.split(": ") for text in raw_stats.split(", ")]
        stats_dict = {stat: int(value) for stat, value in stats}
        sues.update({name: stats_dict})

ticker = dict(
    children = 3,
    cats = 7,
    samoyeds = 2,
    pomeranians = 3,
    akitas = 0,
    vizslas = 0,
    goldfish = 5,
    trees = 3,
    cars = 2,
    perfumes = 1
)

# Part 1

for aunt, stats in sues.items():
    fail = False
    for stat, value in stats.items():
        if ticker[stat] != value:
            fail = True
            break
    
    if not fail:
        print(f"Part 1: {aunt}")
        break

# Part 2:

for aunt, stats in sues.items():
    fail = False
    for stat, value in stats.items():
        if stat in ("cats", "trees"):
            if ticker[stat] > value:
                fail = True
                break
            else:
                continue
        elif stat in ("pomeranians", "goldfish"):
            if ticker[stat] < value:
                fail = True
                break
            else:
                continue
        else:
            if ticker[stat] != value:
                fail = True
                break
            else:
                continue

    if not fail:
        print(f"Part 2: {aunt}")
        break