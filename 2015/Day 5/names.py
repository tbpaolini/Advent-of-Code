import re

# Part 1

nice1 = re.compile(r"(.)\1")
nice2 = re.compile(r"[aeiou].*[aeiou].*[aeiou]")
bad = re.compile(r"ab|cd|pq|xy")

with open("input.txt", "rt") as file:
    santa_list = [line.strip() for line in file]

nice_count = 0
for name in santa_list:
    get_present = (nice1.search(name), nice2.search(name), not bad.search(name))
    if all(get_present):
        nice_count += 1

print(nice_count)

# Part 2

nice3 = re.compile(r"(\D\D).*\1")
nice4 = re.compile(r"(\D)\D\1")

nice_count = 0
for name in santa_list:
    get_present = (nice3.search(name), nice4.search(name))
    if all(get_present):
        nice_count += 1

print(nice_count)
