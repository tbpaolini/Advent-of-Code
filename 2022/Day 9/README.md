## [Day 9 of 2022](https://adventofcode.com/2022/day/9)

I used 2-D arrays (of booleans) to store were both ropes' tails were been. In order to built the arrays, first I simulated the head's movements (which is the same for both ropes), then I got the minimum and maximum coordinates on each axis. Each dimension of the array is the maximum coordinate minus the minimum on that dimension, plus one (in order to account for the coordinate `0`). The indices of coordinate `(0, 0)` on the array are the absolute values of the minimum `(x, y)` coordinates.

Then I performed the actual movement for both ropes. In order to determine the relative position of one knot to the next, I used the taxicab distance (the sum of the absolute differences, on each axis, between the coordinates). Basically, the taxicab distance is how many positions on a grid you need to pass by in order to move (without diagonal movement) between two points. If you ever played Minecraft, you probably heard about the concept).

A taxicab distance of `1` or `0` between two knots means for certain that they touch each other (they are either connected horizontally, vertically, or on the same place). A taxicab distance of `2` means that the knots are either connected diagonally, or separated orthogonally by one space. On this case, if the absolute distance on each axis is `1`, then they are touching, otherwise they are separated. A taxicab distance of `3` or more means for certain that the knots are separated.

The movement of the ropes'head was performed one step at a time, and on each step the position of the knots was updated. If they are separated diagonally, then the knot moves on both axes towards the next knot. In order to determine the direction, the coordinates of the knot were subtracted by the coordinates of the next, then it was checked whether each value is positive, negative, or zero. If the knots were separated orthogonally, then the knot moved towards the next only on the same axis (the direction was also determined by subtraction).

It is worth noting that there is a catch on Part 2 of the puzzle: it is possible for knots `2` to `8` to move diagonally in relation to the next one (the next is the knot with higher index). On Part 1, the movement relative to the next happened on only one axis. This means that if you only check for a taxicab distance of `3` you are going to get the solution wrong. If two knots are connected diagonally, once a knot moves on both axes by 1, the resulting taxicab distance will be `4`. So you should check for a value of `3` or `4` in order to check if a knot should move diagonally (or just for a value greater or equal than `3`, the result will be the same).

Finally, at the end of each step the position of the tails were marked as `true` on the arrays.

**Solution:** [day_09.c](./day_09.c)

**Visualizations:** [output_p1.txt](./output_p1.txt) and [output_p2.txt](./output_p2.txt)