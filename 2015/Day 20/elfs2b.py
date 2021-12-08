import numpy as np

start = np.int32(720720)
end = np.int32(942480)
target = np.int32(33100000)
num_presents = np.int32(11)
num_houses = np.int32(50)
street = np.arange(start, end+1, dtype="int32")
presents = np.zeros_like(street)

prev_max = np.int32(0)
cur_house = np.int32(0)
remaining = np.int32(0)
for elf in np.arange(start // num_houses, end, dtype="int32"):
    visits = (street % elf == 0) * (street <= elf * num_houses)
    presents[visits] += num_presents * elf
    
    max_presents = presents.max()
    if prev_max < max_presents:
        cur_house = np.where(presents == max_presents)[0][0] + start
        remaining = target - max_presents
        prev_max = max_presents
    
    print(f"Elf {elf}: {remaining:<8} (house: {cur_house:<6})", end="\r")
    if max_presents >= target: break

print(f"\nPart 2: {cur_house}")