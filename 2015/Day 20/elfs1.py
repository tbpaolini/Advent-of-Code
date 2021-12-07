import numpy as np
from random import shuffle

def get_divisors(n):
    return {div for div in range(2, n // 2 + 1) if n % div == 0} | {1, n}
    # return [1] + [div for div in range(2, n // 2 + 1) if n % div == 0] + [n]

def get_presents(n):
    return sum(get_divisors(n)) * 10

# Part 1

target = np.uint32(3_310_000)
start = 714_468
end = 840_840
presents = np.zeros(end-start+1, dtype="uint32")
houses = np.arange(start, end+1, dtype="uint32")

for elf in np.arange(1, end+1, dtype="uint32"):
    visits = (houses % elf == 0)
    presents[visits] += elf
    max_presents = presents.max()
    if (max_presents >= target):
        break
    print(f"{elf}: {target - max_presents:<7}", end="\r")

print()
number = houses[(presents >= target)]
print(f"Part 1: {number[0]}")   # Result: 776160


# start = 714_468
# end = 840_840 #850_080 #884_520 #1_000_000
# window = 100
# search_space = set(range(start, end))
# min_house = end

# try:
#     while True:
#         space = list(search_space)
#         shuffle(space)

#         houses = []
#         offset = 0
#         while not houses:
#             for i in range(window):
#                 if i == len(space): break
#                 house_number = space[offset+i]
#                 presents = get_presents(space[offset+i])
                
#                 if presents >= 33_100_000:
#                     houses.append(house_number)
#                 print(f"({offset+i}) House {house_number:<7}: {presents:<8} presents", end="\r")
                
#                 search_space.remove(house_number)
            
#             offset += window
#             if offset >= len(space): break

#         min_house = min(houses)
#         search_space -= set(range(min_house, end))
#         end = min_house
#         print("\n", houses, len(search_space))

#         if len(search_space) <= window: break

# except KeyboardInterrupt:
#     pass

# print(f"\nPart 1: {min_house}")