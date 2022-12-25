## [Day 25 of 2022](https://adventofcode.com/2022/day/25)

This puzzle may look scary at first, but actually the logic of converting decimal from and to other number bases remain the same. The twist here is that the digit's value can be negative, but all operations remain the same.

For converting other bases to decimal:

1. Have an accumulator (initialized to zero) for storing the decimal result.
2. Have an register to store the magnitude of the current digit (initialized to one).
3. Start from the rightmost digit.
4. Multiply the digit's value by the magnitude. Then add the result to the accumulator.
5. Multiply the magnitude by the base size (in our case, `5`).
6. Move to the next digit to the left
7. Repeat steps `4` to `6` until you covered all digits, then return the accumulator.

For converting decimal to other bases:

1. Have an accumulator initialized to the decimal value.
2. Have an string buffer to store the output.
3. Take the modulo of the accumulator by the base size (in our case, `5`).
4. Convert the result of the previous step to the character that represents the value on the other base. In our case, `4` becomes `-`, and `3` becomes `=`. The other results remain the same.
5. Append the character to the left of the string.
6. Subtract the digit's value from the accumulator. Do not forget that `-` has a value of `-1` and `=` a value of `-2` (in which cases, you end up actually adding `1` or `2` to the accumulator).
7. Divide the accumulator by the base size (in our case, `5`). Note: the accumulator should be divisible by the base size, if you did not do anything wrong.
8. Repeat steps `3` to `7` until the accumulator is zero. Then you have the string that represents the number in the other base.

I made a function that converts a SNAFU string to a decimal number, and another function for converting the string back to a decimal number. I parsed the input, converting each line to decimal and adding to the total. Then I converted the total back to SNAFU.

**Solution:** [day_25.c](./day_25.c) (finishes in *3 ms* on my old laptop, when compiled with `-O3`)

Merry Christmas!