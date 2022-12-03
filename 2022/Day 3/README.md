## [Day 3 of 2022](https://adventofcode.com/2022/day/3)

I used bitmasks in order to find which items are in common: the item with priority `n` is stored at bit index `n` of the mask. Since the maximum priority is 52, a 64-bit unsigned integer is enough for the bitmask.

In order to find the item in common, I used the bitwise `AND` operation between the masks. Then I made an assertion to check that only one bit in the mask is set, and I found the index of that bit (by bit shifting the mask to the right until its least significant bit is `1`). That gave the priority of the item in common.

**Solution:** [day_03.c](./day_03.c)