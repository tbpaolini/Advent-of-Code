from itertools import combinations

with open("input.txt", "rt") as file:
    values = []
    for line in file:
        inp, out = line.rstrip().split(" | ")
        values += [(inp.split(), out.split())]

digits = {
    (True, True, True, False, True, True, True)     : 0,
    (False, False, True, False, False, True, False) : 1,
    (True, False, True, True, True, False, True)    : 2,
    (True, False, True, True, False, True, True)    : 3,
    (False, True, True, True, False, True, False)   : 4,
    (True, True, False, True, False, True, True)    : 5,
    (True, True, False, True, True, True, True)     : 6,
    (True, False, True, False, False, True, False)  : 7,
    (True, True, True, True, True, True, True)      : 8,
    (True, True, True, True, False, True, True)     : 9,
}

"""
My trick was to turn the sequences into frozen sets and use them as dictionary keys for the mappings. Then regardless of the order of the elements in the frozen set, it will work to retrieve the mapping from the dictionary.

Identifying which sequence was each digit was the difficult part for me, but here is my logic:

* As stated by the problem, sequences of length (2, 4, 3, 7) can only be digits (`1`, `4`, `7`, `8`) respectively.
* The sequences of length 5 can any of the digits (`2`, `3`, `5`). For those:
    * If the union between two elements has length 7, then the third element can only be digit `3`.
    * For the two remaining elements, it is made the union with digit `4`. If the result has length 7, then it is digit `2`, otherwise digit `5`.
* For the elements of length 6, which can be digits (`0`, `6`, `9`):
    * If the union of the element with digit `1` has length 7, then it can only be digit `6`.
    * For the two remaining, if it has exactly one non-overlapping segment with digit `5`, then it can only be digit `9`. Otherwise digit `0`.

That should map all 10 sequences to a different digit. Then it is only to use the mappings to decode the sequences on the output, and sum the results.
"""

# Part 1

count = 0
for (inp, out) in values:
    for value in out:
        size = len(value)
        if size in (2, 4, 3, 7): count += 1

print(f"Part 1: {count}")

# Part 2

sum_display = 0

for (inp, out) in values:
    mapping = {frozenset(signal): None for signal in inp}
    temp_len5 = set()
    temp_len6 = set()
    
    for signal in inp:
        size = len(signal)
        ID = frozenset(signal)
        if   size == 2: mapping[ID] = 1
        elif size == 4: mapping[ID] = 4
        elif size == 3: mapping[ID] = 7
        elif size == 7: mapping[ID] = 8
        elif size == 5: temp_len5.add(ID)
        elif size == 6: temp_len6.add(ID)
    
    assert len(temp_len5) == 3
    assert len(temp_len6) == 3

    for pair in combinations(temp_len5, 2):
        s1, s2 = pair
        if len(s1 | s2) == 7:
            (s3,) = (temp_len5 - {s1, s2})
            mapping[s3] = 3
            digits_2_or_5 = {s1, s2}
    
    digits = {dig: ID for ID, dig in mapping.items() if dig}
    
    for pair in combinations(temp_len5, 2):
        s1, s2 = pair
        if len(s1 | s2) == 7:
            (s3,) = (temp_len5 - {s1, s2})
            mapping[s3] = 3
            digits_2_or_5 = {s1, s2}
            break
    
    s1, s2 = digits_2_or_5
    if len(s1 | digits[4]) == 7:
        mapping[s1] = 2
        mapping[s2] = 5
        digits[2] = s1
        digits[5] = s2
    else:
        mapping[s1] = 5
        mapping[s2] = 2
        digits[5] = s1
        digits[2] = s2
    
    for signal in temp_len6:
        if len(signal | digits[1]) == 7:
            mapping[signal] = 6
            digits[6] = signal
            digits_0_or_9 = temp_len6 - {signal}
            break
    
    s1, s2 = digits_0_or_9
    if len(s1 - digits[5]) == 1:
        mapping[s1] = 9
        mapping[s2] = 0
        digits[9] = s1
        digits[0] = s2
    else:
        mapping[s1] = 0
        mapping[s2] = 9
        digits[0] = s1
        digits[9] = s2
    
    assert len(mapping) == 10
    assert len(digits) == 10

    sum_display += sum(
        mapping[frozenset(signal)] * (10**n)
        for n, signal in zip(range(3, -1, -1), out)
    )

print(f"Part 2: {sum_display}")