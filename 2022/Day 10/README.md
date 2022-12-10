## [Day 10 of 2022](https://adventofcode.com/2022/day/10)

I kept track of a cycle count and a cooldown value to determine when an operation is finished. `addx` set the cooldown to `2` and set the value to be added to what the instruction specified. `noop` set the cooldown to `1` and the value to be summed to `0`. The cooldown decremented by `1` each cycle, and when it reached `0` the value was summed to the register X. Then the next instruction was parsed.

The order of operations matters. The addition to the register X is performed at the end of the cycle. So the signal strength check and pixel drawing are done before the register addition. The screen coordinate to draw the pixel was calculated from the cycle counter:

* The top left of the screen is coordinate `(x = 0, y = 0)`
* `x` increases from left to right, `y` increases from top to bottom
* The cycle counter starts from 1
* `x = (cycle - 1) % 40`
* `y = (cycle - 1) / 40`

The pixel is lit if `y-1 <= register_x <= y+1`, because the sprite has a width of `3`.

**Solution:** [day_10.c](./day_10.c)