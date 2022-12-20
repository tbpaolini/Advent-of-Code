## [Day 18 of 2022](https://adventofcode.com/2022/day/18)

In order to represent the cubes, I used a 3-D array of booleans.

*Part 1:* In order to check if a cube face was exposed to air, I iterated over all cubes and checked if the adjascent spaces along the axes were empty.

*Part 2:* For each face exposed to air, I checked if there was a path to the outside of the 3-D area. For that, I performed a [Depth-First Search](https://en.wikipedia.org/wiki/Depth-first_search). From the starting point, all unvisited exits are checked in the order they are seen. The same is made for each exit, until a path or a dead end is found. It is necessary to keep track of the exits that were already visited, otherwise you might end in an infinite loop.

**Solution:** [day_18.c](./day_18.c) (finishes in *241 ms* on my old laptop, when compiled with `-O3`)