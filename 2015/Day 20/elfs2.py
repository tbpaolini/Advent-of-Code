# import numpy as np

def get_presents(house):
    return sum([n for n in range(1, house//2+1) if (house % n == 0) and (n * 50 >= house)] + [house]) * 11

def divisors(n):
    return [div for div in range(1, n+1) if n % div == 0]

def divisors_50(n):
    return [div for div in range(1, n//2+1) if (n % div == 0) and (div * 50 >= n)] + [n]

# Part 2

target = 33100000
houses = []

for i in range(942480, 720720, -10):
    print(f"House: {i}", end="\r")
    if get_presents(i) >= target:
        houses += [i]
        print(f"Found: {i}")
        with open("output (part 2).txt", "at") as file:
            file.write(f"{i}\n")

print(f"\nPart 2: {min(houses)}")   # Result: 786240

# # Part 2

# target = np.uint32(33_100_000)
# start = 471_240
# end = 942_480
# presents = np.zeros(end-start+1, dtype="uint32")
# houses = np.arange(start, end+1, dtype="uint32")
# num = np.uint32(11)

# # 471,240 elfs --> house #942,480 = 40,459,650 presents
# # 720720 - too low
# # 942480 - too high

# for elf in np.arange(1, end+1, dtype="uint32"):
#     visits = np.where(houses % elf == 0)[0][:50]
#     presents[visits] += elf * num
#     max_presents = presents.max()
#     print(f"{elf}: {target - max_presents:<8}", end="\r")
#     if (max_presents >= target):
#         break

# print()
# number = houses[(presents >= target)]
# print(f"Part 2: {number[0]}")