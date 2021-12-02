from itertools import permutations

locations = set()
distances = {}
with open("input.txt", "rt") as file:
    for line in file:
        local1, local2, distance = line.replace(" to", "").replace(" =", "").split()
        places = (local1, local2)
        locations.update(places)
        distances.update({places: int(distance)})

distances_list = []
route_size = len(locations)
for route in permutations(locations, route_size):
    
    distance = 0
    for step in range(route_size-1):
        places = (route[step], route[step+1])
        try:
            distance += distances[places]
        except KeyError:
            places = tuple(reversed(places))
            distance += distances[places]
    
    distances_list.append(distance)

# Part 1
print(min(distances_list))

# Part 2
print(max(distances_list))