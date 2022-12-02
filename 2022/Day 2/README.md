## [Day 2 of 2022](https://adventofcode.com/2022/day/2)

Instead of checking for every possible combination, I enumerated the plays (`rock = 0`, `paper = 1`, `scissors = 2`). Then checked if the other's play was the same, the next, or the previous on the sequence. That results in a draw, win, or loss (respectively).

In order to parse the plays, I have read the file line by line, and I took the ASCII values of the first and third characters of each line (the other's and yours play). Then I subtracted those by the ASCII values of `A`and `X`(respectively), which results in the value of the play.

I made a function that calculates the score according to the rules. For part 2, I used a `switch`statement to force your play to be the same, the previous, or the next in the rock-paper-scissors sequence.

I stumbled here at first, when taking the modulo to wrap around the sequence. I was using 8-bit unsigned integers. With those, `0 - 1` becomes `255`. So `255 % 3 = 0` (instead of `2`). I solved that by adding `3`  before calculating the modulo.

**Solution:** [day_02.c](./day_02.c)
