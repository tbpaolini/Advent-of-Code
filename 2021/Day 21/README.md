# Solution - Advent of Code 2021, Day 21

Part 1 is very straightforward. Perhaps the one pitfall one may get into is that the position is numbered from 1, instead of 0. So in order to cycle back to the beginning is necessary to subtract 1 from the position before doing the modulo, and then adding 1 to the result.

For the part 1's dice, I made an infinite generator that keeps repeating integers from 1 to 100 infinitely. Then I pulled 3 values at a time from it. The rest was trivial: move the player, calculate the score, alternate between players until someone reaches 1000 points.

Part 2, on the other hand, was more much complicated. I noticed beforehand that it was unfeasible to simulate every last of the trillions of possibilities, that it was necessary to keep track of the win counters in some other form. But I could not figure out initially how to do it. I looked on Reddit for ideas, and managed to understand the strategy. Credits go to u/SwampThingTom for the idea ([this comment](https://www.reddit.com/r/adventofcode/comments/rl6p8y/comment/hpgdx9o/), and [the code](https://github.com/SwampThingTom/AoC2021/blob/main/Python/21-DiracDice/DiracDice.py) behind it). After understanding the approach, I implemented it on my code (instead of copy/pasting).

The function that calculates the outcome of the games is deterministic. That is, for the same initial conditions the win counts will be the same. So the results can be cached, and returned when the function is called again with the same parameters (saving a lot of processing time).

On the top of that, what matters in each turn is the sum of the 3 rolls, not the value of each dice. So first we pre-calculate all possible combinations of 3 dices, and then count how many times a specific sum appears. Then each sum only needs to be checked once, and then the outcome is multiplied by the number of times that the sum appears. This saves even more processing time.

All in all, what I did for part 2 (using recursion):

* In a turn, iterate over all possible sums and calculate the player's current position and score.
* For each result, iterate over all possible sums for the other player, then calculate their position and score.
* Keep repeating the process alternating between players until someone reaches 21 points.

Then the function returns 1 for who won and 0 for who lost. This value is multiplied by the number of times that the sum appears, and added to the win counts of the function. The function returns those counters for their callers, after all sums have been iterated. So the win counts are propagated all the way to the original function call, which returns the aggregate win counts of all possible game combinations.

This code took 0.5 seconds to run on my computer. This time was surprisingly low for me, considering that literally trillions of games were accounted precisely. It was fast because of the results caching (or memoization, if you want the technical term) and aggregating the sums together.

**My code:** [Parts 1 and 2](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2021/dirac-dice.py)