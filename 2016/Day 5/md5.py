from hashlib import md5
from itertools import count
from time import perf_counter

with open("input.txt", "rt") as file:
    door = file.readline().strip()

# Part 1

password = ""
start_p1 = perf_counter()
for value in count():
    sequence = (door + str(value)).encode("ascii")
    my_hash = md5(sequence).hexdigest()
    if my_hash.startswith("00000"):
        password += my_hash[5]
        print(password, end="\r")
        if len(password) == 8: break
total_p1 = perf_counter() - start_p1

print(f"Part 1: {password} (took {total_p1:.1f} seconds)")

# Part 2

password_p2 = [""] * 8
start_p2 = perf_counter()
for value in count():
    sequence = (door + str(value)).encode("ascii")
    my_hash = md5(sequence).hexdigest()
    if my_hash.startswith("00000"):

        pos = my_hash[5]
        if not pos.isnumeric(): continue

        pos = int(pos)
        if pos >= 8: continue

        if not password_p2[pos]: password_p2[pos] = my_hash[6]
        pass_str = "".join(char if char else "_" for char in password_p2)
        print(pass_str, end="\r")
        
        pass_len = sum(len(char) for char in password_p2)
        if pass_len == 8: break

total_p2 = perf_counter() - start_p2

print(f"Part 2: {pass_len} (took {total_p2:.1f} seconds)")