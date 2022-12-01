# [Day 1 of 2022](https://adventofcode.com/2022/day/1)

Last year I used Python, this time I am upping the challenge by using C. This is an opportunity for coding myself the data structures and algorithms, as base language do not have too many out of the box.

The basic logic of this solution is not difficult:

* read the file line by line
* convert the text to an integer and sum it to the current value
* switch to the next value when an empty line is found
* compare all values at the end

What I did was to keep track of the current top 3 values. I think that it would be faster if I stored all values, then sorted them only at the end. But the current solution already works fast enough and I find it simpler, as it is not necessary to deal with dynamic arrays or count beforehand the amount of elves.

**Solution:** [day_01.c](./day_01.c)
