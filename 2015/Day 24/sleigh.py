from math import prod
from time import perf_counter

with open("input.txt", "rt") as file:
    presents = [int(line) for line in file]

presents.sort(reverse=True)

def min_subset_sum(target, time_limit=5):
    global presents
    sums = []
    min_size = len(presents)

    def sum_weights(partial=[], pool:list=presents):
        nonlocal sums, min_size
        partial_sum = sum(partial)
        if partial_sum > target: return
        if perf_counter() - time_start > time_limit: return
        
        size = len(partial)
        if size > min_size: return
        if partial_sum == target:
            if size < min_size: sums.clear()
            sums.append((partial, prod(partial)))
            min_size = size
            return
        
        for i, value in enumerate(pool):
            new_pool = pool.copy()
            new_pool.remove(value)
            sum_weights(partial + [value], new_pool)
    
    try:
        time_start = perf_counter()
        print(f"Searching combinations for {time_limit} seconds...")
        sum_weights()
    except KeyboardInterrupt:
        pass
    return sums

part1_weight = sum(presents) // 3
combinations_p1 = min_subset_sum(part1_weight)
front_combination_p1 = min(combinations_p1, key=lambda f: f[0][1])
print(f"Part 1: {front_combination_p1[1]}")     # 11266889531

part2_weight = sum(presents) // 4
combinations_p2 = min_subset_sum(part2_weight, time_limit=20)
front_combination_p2 = min(combinations_p2, key=lambda f: f[0][1])
print(f"Part 1: {front_combination_p2[1]}")     # 77387711