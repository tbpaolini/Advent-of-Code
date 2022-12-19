## [Day 17 of 2022](https://adventofcode.com/2022/day/17)

In order to represent the board, I used an array of bytes. Since the board has an width of 7 blocks, 8-bits are enough to represent with bitmasks which spaces on the row are filled. The size of the array was 4096 bytes. If a piece would cause the array to overflow, then I kept the 1024 newest rows, and deleted the rest. I kept track of how many rows were trimmed in total, in order to calculate the total height.

The pieces were also represented with bitmasks. Checking if a piece would collide with a block on the board was a matter of doing a bitwise `AND` with the values on the array, and landing a piece was done with a bitwise `OR`. Moving a piece horizontally is a bitwise shift to the left or the right. All of this is easier said than done, but after a magical journey full of segmentation faults and off-by-one errors, I managed to get it working. :-)

Now for part 2, where a trillion pieces need to be dropped. Simulating every single one of them would take over 4 months (after a rough estimation), so it is necessary a more efficient approach. We need to find a period in which the board begins to repeat itself, then get the amount of periods in a trillion pieces. That amount is multiplied by how much the height increases after a period, then the remaining pieces are simulated normally.

Both the pieces and the horizontal movements cycle after they get to the end. The length of the period is the least common multiple between the amount of pieces and the amount of horizontal movements. Since the board initially is not at the beginning of a period, I first simulated one period from the start, in order to guarantee that the board is at the beginning of a period. I do not know how to prove that this will always be the case, but it worked on both the test and input data.

From this point, I kept track of how much the total height changed after another period. Then I checked for when the height changes started to repeating: I had a sample window of 5 values, then I checked if this sequence at the end also appeared anywhere before. If it did, then I considered the period as found. Had this not worked to get the correct results, I would have increased the sample window.

To get the final result, I first simulated one movement cycle. Then I calculated how many periods fit in the remaining pieces, and multiplied that by the amount of pieces in a period. Finally, the pieces that were still remaining after that were simulated. The height after these 3 steps combined is the solution. It took a little over than four seconds for me, which beats waiting for months. :-)

**Solution:** [day_17.c](./day_17.c) (finishes in *4.32 s* on my old laptop, when compiled with `-O3`)

*Note: For timing, I have considered everything from both parts combined, including finding a cycle (which is the slowest operation).*