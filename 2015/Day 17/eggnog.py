from itertools import combinations, combinations_with_replacement

with open("input.txt", "rt") as file:
    values = [int(line.strip()) for line in file]

def count_combinations(target=150, numbers=values):
    count = 0
    combinations = []
    def sum_values(target, numbers, containers=[]):
        nonlocal count, combinations
        my_sum = sum(containers)
        if my_sum == target:
            count += 1
            combinations.append(len(containers))
            return
        if my_sum > target: return
    
        for i, value in enumerate(numbers):
            sum_values(target, numbers[i+1:], containers + [value])
    
    sum_values(target, numbers)
    return count, combinations.count(min(combinations))
    # Source: https://stackoverflow.com/questions/4632322/finding-all-possible-combinations-of-numbers-to-reach-a-given-sum

total_combinations, min_combinations = count_combinations()
print(f"Part 1: {total_combinations}")
print(f"Part 2: {min_combinations}")