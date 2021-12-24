# Solution - Advent of Code 2021, Day 22

Part 1 was relatively easy, when compared to Part 2. For Part 1, I just had a set of all coordinates that were "on". I only updated that set for coordinates that fell in the range of -50 to +50 (on all 3 axes). Then I just counted how many elements there were in the set.

Part 2 was the biggest challenge. The approach of Part 1 would not work because it would require some inordinate amount of memory to store trillions of coordinates. It took me 2 days to finish it. I could realize that the blocks that are "on" needed to be split into new blocks, but it was difficult for me to figure out how exactly to code that. It is the sort of thing that it goes completely wrong if you miss a sign or swap a coordinate, which is easy to happen and debugging it can be a nightmare.

But in the end it worked, and here is how I did it. All existing "on" blocks that were fully enclosed in the instruction region were excluded. The blocks that were fully outside the region were left untouched. The blocks that were partially intersected by the region were split (the parts outside the region became new blocks, the parts inside were removed). Finally, if the instruction was to "turn on", then the region itself was also added to the list.

I recognize that it is difficult to visualize how to accomplish all of this programmatically, or at least it was for me. I guess that it helps if you have some cube-shaped objects, or a 3D modeling software, it might help to see what is happening.

Tho cuboids overlap when their projections on all 3 axes also overlap. More specifically, on this puzzle, you need to check (for each axis) if the minimum coordinate of one cuboid is smaller or equal than the maximum coordinate of the other cuboid. They overlap only if those check pass for ***all axes*** and ***both cuboids***.

To split the intersections, the parts above faces of the region are checked. On each axis the minimum coordinates of one region becomes the maximum coordinate of the block, and the minimum coordinate of the region becomes the maximum coordinate of the block. Also one needs to the added or removed to the new coordinate (depending on the direction of the face), because the blocks within the region needs to be excluded.

Then it is just necessary to sum the volume of all blocks in the list.

**Code:** [Part 1](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2022/cubes_part1.py) and [Part 2](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2022/cubes2.py)

**Bonus:** [2D visualization of an intersection and split](https://github.com/tbpaolini/Advent-of-Code/raw/master/2021/Day%2022/image/intersection.png)