## [Day 24 of 2022](https://adventofcode.com/2022/day/24)

I used Breadth-First Search in order to find the shortest path. Since the exits on the map changes dynamically, it is possible to return to a previously visited position, or even wait for some time at the current position. This poses a challenge when keeping track of where you have been before, so you do not keep walking in circles without getting anywhere. What I did was to consider each of the different map states as a separate map, for the purpose of keeping track of the previous positions.

Since the blizzards loop around, there is a limited amount of map states. This amount is the *least common multiple* (LCM) of the width and height of the area where the blizzards are. Their area on the complex example is `4 x 6`. The LCM of the dimensions is `12`, so in the example the blizzards return to their initial positions every 12 minutes. You can verify on the example that **minute 12 has the same pattern as minute 0**, minute 13 the same as minute 1, and so on.

On the input, the blizzard's area is `150 x 20`, in this case the LCM is `300`. So **the input map has 300 different states** that need to be kept track of. We can consider those states as being the layers of a 3-D map, with the top and bottom wrapping around to each other. This ways, we can see the movement as always "moving down" to the next layer as we take steps.

In order to pathfind, I initialized two 3-D arrays of `300 x 20 x 150`: one with all blizzard positions through all states, and another for keeping track of the exits that were already seen across all states. Since the blizzards wrap around the corners, their positions after some time can be calculated by taking their initial positions (subtracting the walls), adding the minute times the direction, then taking the modulo by the size of the blizzard's path (the walls are added back afterwards).

The Breadth-First Search algorithm uses a queue (first in, first out), in order to decide which exits to check next. In other words, the nodes that were seen the earliest are visited as soon as possible (contrast with Depth-First Search, that does the other way around). In our case, the twist is that our 2-D map became a 3-D map with all blizzard states, so the exits are always "down" the current node. My implementation goes like that:

1. Set the current node as the starting position.
2. From the current node, check which of the 5 exits have no blizzards (waiting on the same spot is considered an exit).
3. For each unblocked exit, if that exit node has not been seen yet, flag it as `seen` then push it to the end of the queue.
4. Pop the first exit from the start of the queue, then set it as the current node.
5. Repeat steps `2` to `4` until the destination node is set as the current node.

If the queue becomes empty before the destination node is reached, that means no path exists (which should not happen in our case).

Parts 1 and 2 of the problem work pretty much the same way. It is just necessary to remember that in Part 2, you do not begin from minute zero, and that the start and end positions are switched:

* From the minute you ended on Part 1, calculate a path from the end to the start.
* Then from the minute you ended, calculate a path from the start to the end.

**Solution:** [day_24.c](./day_24.c) (finishes in *187 ms* on my old laptop, when compiled with `-O3`)

As a bonus, I would like to explain the algorithm I used to find the LCM of two values:

1. Sort the two values
2. Initialize a `LCM` accumulator to `1`.
3. Loop over all numbers from 2 to the smallest value.
4. Using modulo, test if both values are divisible by the loop number.
5. If so, then divide both values by the number, and multiply the `LCM` by it.
6. The loop can exit early if at any point the tested number is greater than the current small value.
7. After the loop finishes, multiply the `LCM` by the current big and small values.

And at this point the `LCM` accumulator contains the least common multiple of the initial values.