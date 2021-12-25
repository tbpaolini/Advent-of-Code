# Solution - Advent of Code 2021, Day 25

This is the kind of problem that NumPy makes much easier to solve. NumPy can make easily batch operations on arrays, like moving elements around, testing everything against a condition, and addressing the elements by a mask.

Here is what I did. In a 2D array for the positions, I used 0 to represent "empty", 1 for "east-facing", and 2 for "south-facing". For each step:

1. Checked which spots were not empty
2. Checked which spots had an east-facing sea cucumber
3. Rolled the east positions by one element to the right, in order to simulate the tentative movements
4. Compared the resulting array with the array of the initial positions: *east destination array* `AND NOT` *starting positions array*
5. I set the destination position to the east value
6. I rolled back the destination array in order to get the starting positions, and then I set those to "empty"
7. Checked again for which spots were empty
8. Repeated steps `2` to `6` for the south movement
9. Counted how many sea cucumbers could move (just added all the `True` on their destination arrays)
10. Stop if the movement count is 0, otherwise restart from step `1`

I considered this puzzle easy, when compared to the previous couple of days, but that can change from person to person. Since I was already familiar with NumPy, that helped me a lot. Also the code took 150 milliseconds to finish on my (old) computer.

**Code:** [Solution for Day 25 of 2021](https://github.com/tbpaolini/Advent-of-Code/blob/master/2021/Day%2025/grand_finale_2021.py)

**<font color="green">Merry</font> <font color="red">Christmas</font>!**