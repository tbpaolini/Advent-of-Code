## [Day 5 of 2022](https://adventofcode.com/2022/day/5)

This day is where it gets more interesting to solve in C, because it is necessary to manualy code a stack. I used a linked list for each of the stacks, with an array to keep track of the top element on each one. The boxes are linked to the ones below it. Then moving the boxes is a matter of changing their pointers to another box.

*Part 1:* In order to move a single box, the pointer of the top of the origin stack is changed to the box below. The moved box then is linked to the top of the destination stack. And finally the pointer to the top of the destination stack is changed to point to the new box.

*Part 2:* Moving multiple boxed is similar, except that first we need to navigate through the next elements on the stack in order to find the bottom box of the pile. Then the bottom of the pile is linked to the top of the destination stack. The top of the origin stack is changed to the box below the pile. And the top of the destination pile is changed to the top box of the pile.

**Solution:** [day_05.c](./day_05.c)