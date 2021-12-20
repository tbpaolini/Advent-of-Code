# Solution - Advent of Code 2021, Day 20

The catch of this puzzle is that, unlike in the example, 9 unlit pixels decode into a lit pixel. Therefore the infinite size does matter, so it is a bit more complicated than considering the neighboring pixels as always being unlit.

In our case, 9 unlit pixels become a lit pixel, while 9 lit pixels become an unlit pixel. So the infinite grid alternates between "on" and "off" each cycle. The puzzle always asks for an even number of steps, because otherwise it would fall in an unlit cycle and the sum would be infinite.

But we cannot forget that the original image area is going to affect its neighboring pixels each step. Since we cannot actually store an infinite image in memory, the big trick to solve the puzzle is to figure out how far away from the original image we need to consider.

For each step, the image affects its first neighbors. But those neighbors also affect their own first neighbors. We do not need to consider further that that because all pixels beyond that because they will always be all lit or all unlit. So the total area we need to consider is two pixels per step in all directions (4 pixels for Part 1, 100 pixels for part 2).

What I did was to apply the algorithm to an image array that has been initially padded with zeroes (by the amount explained above). Then I looped through all 3 x 3 areas in the array, then I converted the area from binary to decimal, and I looked on the algorithm which value the pixel corresponds to.

After finishing all the steps, I removed the padded area to the right and bottom. I did that because when looking through the 3 x 3 area  around a pixel, the outermost pixels are left without being "decoded". This creates artifacts around the borders. Since my output array was filled starting from top left, then the artifacts were located only around the bottom right corner.

No, I didn't visualize that initially. I figured that out because during the testing phase I generated the intermediate images. And then I looked for a pattern in the artifacts. So I adjusted my code and got the correct result for the pixels count.

**Code :** [Part 1 & 2](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2020/trenchmap.py)

**Bonus:** [Images that I generated](https://github.com/tbpaolini/Advent-of-Code/tree/master/2021/Day%2020/images)