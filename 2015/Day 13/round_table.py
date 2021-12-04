from itertools import permutations

with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2015\Day 13\input.txt", "rt") as file:
    
    friendship = {}
    people = set()
    for line in file:
        cmd = line.strip("\n.").split()
        name1 = cmd[0]
        sign = 1 if cmd[2] == "gain" else -1
        change = int(cmd[3])
        name2 = cmd[-1]
        
        friendship.update({(name1, name2): sign * change})
        people.update((name1, name2))

# Part 1

def best_happines():
    amount = len(people)
    friendship_totals = []
    
    for arrangement in permutations(people, amount):

        total = 0
        for i in range(amount-1):
            pair = tuple(arrangement[i : i+2])
            rev_pair = tuple(reversed(pair))
            total += friendship[pair]
            total += friendship[rev_pair]
        
        pair = (arrangement[amount-1], arrangement[0])
        rev_pair = tuple(reversed(pair))
        total += friendship[pair]
        total += friendship[rev_pair]

        friendship_totals.append(total)

    print(max(friendship_totals))

best_happines()

# Part 2

for person in people:
    pair1 = ("Tiago", person)
    pair2 = (person, "Tiago")
    friendship.update(
        {
            pair1: 0,
            pair2: 0
        }
    )

people.add("Tiago")

best_happines()