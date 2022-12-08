## [Day 8 of 2022](https://adventofcode.com/2022/day/8)

I used a 2-D array to store the heights of the trees, and another for keeping track of the visible trees. I also kept track of the highest tree of each one of the rows and columns.

The naive solution would be to iterate over every single tree, then check if all trees in each of the 4 directions are smaller. Needless to say, that would be very inefficient. It is more efficient to check the trees starting from each of the four borders, then stopping when you reach the highest tree on the row or column.

That is what I did. And before incrementing the counter of visible trees, I checked if the tree had been already flagged as visible. This way I avoided counting each tree more than once.

After that, I iterated over the visible trees and counted how many trees (in the 4 directions) until the next tree with at least the same height. And finally, the tree's score was calculated by multiplying the 4 values.

**Solution:** [day_08.c](./day_08.c)