## [Advent of Code: Day 6 of 2023](https://adventofcode.com/2023/day/6)

[Language: C]

From the start I already decided to calculate the minimum and maximum held times, instead of brute-forcing each possibility. Brute force would have worked for Part 1, but not so much for Part 2 due to the larger scale.

Let `T` being the time, `D` the distance, and `x` how long the button was held. In order for us to break the record of a race, the following inequality has to be satisfied:

```
(T - x) * x > D
```

If we [solve it for x](https://www.wolframalpha.com/input?i=solve+for+x%3A+%28T+-+x%29+*+x+%3E+D), we get:

```
(T - sqrt(T*T - 4*D)) / 2  <  x  <  (T + sqrt(T*T - 4*D)) / 2
```

Then we just need to count all the integer values from that range. If the minimum value is already an integer we add 1 to it, otherwise we ceil it. If the maximum value is already an integer we subtract 1 of it, otherwise we floor it. After that, in order to get the integer count we subtract the min value from the max value, and add 1.

**My solution:** [day_06.c](https://github.com/tbpaolini/Advent-of-Code/blob/master/2023/Day%2006/day_06.c)
