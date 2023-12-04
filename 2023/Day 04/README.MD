## [Advent of Code: Day 4 of 2023](https://adventofcode.com/2023/day/4)

[Language: C]

In order to keep track of the numbers for each card I used an array of booleans: the index represents the number, if it is present on the card then its value is `true`. Since there numbers go up to 99, the array had 100 elements. I also had array to keep track of how many winning numbers each card have, and another for how many of each card there is. When each card was parsed, its count was incremented by 1 and its amount of winning numbers were stored. Needless to say, I performed bounds check on each array operation.

For **Part 1**, the card's score was `1 << (win_count - 1)` if its amount of winning numbers was greater than zero, otherwise the score was zero. Then all scores were added together.

For **Part 2** I had an outer loop that went over each card once, and an inner loop that went through the obtained cards following the current card. The inner loop added the amount of the current card to each of the obtained card. That is because if you have `n` of a card with `m` winning numbers, then it is going to win `n` times, and each of those wins gets you 1 new card for each of the following `m` cards. After all cards were processed, I added together the final count of each card.

**My solution:** [day_04.c](https://github.com/tbpaolini/Advent-of-Code/blob/master/2023/Day%2004/day_04.c)
