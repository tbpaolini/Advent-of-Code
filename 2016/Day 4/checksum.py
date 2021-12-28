from __future__ import annotations
from collections import Counter
import re
from string import ascii_lowercase

data_regex = re.compile(r"([a-z\-]+)-(\d+)\[(\D{5})\]")

with open("input.txt", "rt") as file:
    rooms:list[tuple[str]] = data_regex.findall(file.read())

id_sum = 0
real_rooms:list[tuple[str, int]] = []
for code, ID, checksum in rooms:
    
    letters_count = Counter(code.replace("-", ""))
    letters_sort = sorted(
        letters_count,
        key = lambda f: (-letters_count[f], f),  # Sort ascending for the negative of the count, and then alphabetically
    )

    my_checksum = "".join(char for char in letters_sort[:5])

    if my_checksum == checksum:
        id_sum += int(ID)
        real_rooms += [(code, int(ID))]

print(f"Part 1: {id_sum}")

decoded = {}
alphabet = ascii_lowercase
for code, ID in real_rooms:
    
    plain_text = ""
    for char in code:
        
        if char.isalpha():
            pos = alphabet.index(char)
            new_pos = (pos + ID) % 26
            plain_text += alphabet[new_pos]
        
        else:
            plain_text += char
    
    decoded[plain_text] = ID

print(f"Part 2: {decoded['northpole-object-storage']}")