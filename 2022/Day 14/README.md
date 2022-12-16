## [Day 14 of 2022](https://adventofcode.com/2022/day/14)

In order to represent the map, I used a 2-D array which each element can have 4 values: `EMPTY` (0), `WALL` (1), `SAND` (2), or `SOURCE` (3).

I have calculated the size of the array in order to be enough to fit the map for both parts. I have calculated the maximum and minimum absolute coordinates, in order to get the width and height of the map. Then I added two spaces to the right, left, and bottom. And I also expanded the width by the value of the height, because when the sand fills the map to the top it forms a triangle that extends from the center by one unit to each side as it goes down. Finally, I calculated the offset to convert the absolute coordinates to array coordinates.

When "drawing" the walls, it is important to notice that the start and end coordinates can go to either direction (from highest to smallest, or from smallest to highest). If one assumes that the coordinates always go in the same direction, then some of the walls will not be drawn.

I made the simulation for both parts at the same time on the same map. When the sand went to the bottom by the first time I took note of the value (part 1's solution). When the sand stopped at the same coordinate of the source, I stopped and took note of the count (part 2's solution).

There are some optimizations that could be made. It is not necessary to simulate the whole process of the sand falling down, as it follows the same path of the previous sand until before the later landed. So it is possible to "simulate backwards" the fall of the previous sand, and place the next there, and then continue from that point. I did not do that because I was already happy with the overall speed of the program (21 ms to do both parts), and it would increase the complexity of the code. If it ain't broken, don't fix. :-)

**Solution:** [day_14.c](./day_14.c) (finishes in *21 ms* on my old laptop, when compiled with `-O3`)

**Visualizations:** [map_part1_start.txt](./map_part1_start.txt), [map_part1_end.txt](./map_part1_end.txt), [map_part2_start.txt](./map_part2_start.txt), [map_part2_end.txt](./map_part2_end.txt).