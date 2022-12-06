## [Day 6 of 2022](https://adventofcode.com/2022/day/6)

In order to check if all characters in a sequence are unique, I used an array of 26 booleans (initialized to `false`) to check which characters have been seen already. The index of a character in the array is its code point minus the code point of `a` (I checked beforehand if the character was a lowercase letter).

Then for each character in a sequence, if it was not seen before then it is flagged as `true`. Otherwise, it means that the character was repeated. Only when after all characters have passed the check, we have a valid marker (all unique characters).

What changes between part 1 and 2 is the size of the unique sequence, but the logic to validate it stays the same.

**Solution:** [day_06.c](./day_06.c)