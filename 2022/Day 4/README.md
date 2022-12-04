## [Day 4 of 2022](https://adventofcode.com/2022/day/4)

*Part 1:* For In order for a section to be fully inside another, the lower bound of it must be greater or equal than the lower bound of the other, and its upper bound must be less or equal than the lower bound of the other.

*Part 2:* In order to check if two sections intersect at any point, I find easier to check if they do not intersect at all and then invert the result. They do not intersect if the upper bound of one is smaller than the lower bound of the other.

**Solution:** [day_04.c](./day_04.c)