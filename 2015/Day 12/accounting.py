import re

with open("input.txt", "rt") as file:
    raw_file = file.read()

# Part 1

numbers = re.compile(r"(-*\d+)")
total = 0

for number in numbers.findall(raw_file):
    total += int(number)

print(total)

# Part 2

# corrected_file = re.sub(r'{.*?:"red".*?}', "", raw_file)
# total = 0

# for number in numbers.findall(corrected_file):
#     total += int(number)

# print(total)

import json

json_file = json.loads(raw_file)

def parse_json(contents):

    total = 0
    if isinstance(contents, dict):
        if "red" in contents.values():
            return 0
        else:
            contents = contents.values()

    for entry in contents:
        
        if isinstance(entry, list) or isinstance(entry, dict):
            total += parse_json(entry)
            continue

        try:
            total += int(entry)
        except ValueError:
            pass
    
    return total

print(parse_json(json_file))
