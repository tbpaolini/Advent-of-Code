## [Day 20 of 2022](https://adventofcode.com/2022/day/20)

In order to represent the values, I used a circular doubly linked list (the values are linked to their previous and next forming a cycle). I also had an array with nodes of that list, but in the original order that the values appear. And I stored on a temporary variable the pointer to the node with the value of `0`.

Moving a node by `N` positions on the circular list is equivalent to removing the node, rotating the list by `N`, then inserting the element there. When rotating the list it might be easy to commit the mistake of taking the modulo of `N` by the original amount of values, but you actually need to modulo by **the original amount minus `1`**, because the other values are rotating around the moved value. Doing this will reach the same destination, but potentially with less steps.

Another optimization is to check which direction to rotate the list, while still reaching the same destination. To get the amount of steps in the other direction, we sum the **original amount minus 1** with the **current step count**. Then we compare which step count has the **smallest absolute value**. And we rotate the list in that direction. If positive, the list is rotated forwards. If negative, the list is rotated backwards. If zero, the list remains unchanged.

Algorithm to move a node by `N` steps:

1. Set the list's head to the node that will be moved (the original node).
2. Set the previous node's `next` to the head's `previous`.
3. Set the next node's `previous` to the head's `next`.
4. Move the list's head to its next node (*operations 1 to 4 remove the original node from the list*).
5. Move the list's head by `N` positions (keep moving to its `next` node if rotating forwards, to the `previous` node otherwise).
6. Set the original node's `previous` to the head.
7. Set the original node's `next` to the head's `next`.
8. Set the `previous` of the *node after the head* to the original node.
9. Set the `next` of the head to the original node (*operations 6 to 9 insert the original node after the current head*).

I took a long time to get this right for two reasons. First, was because I did not realize that it is necessary to take the modulo of the list's length minus one. And second, because I was trying to move the value in-place (without removing it first). That ran into weird edge cases, when the value ended just before or after its original position (since in this case 3 nodes need to have their links updated, rather than 4 nodes). In the end, it was simpler to just remove the node, then insert it back on the new position (that always works as long the list has more than two elements).

In order to check the values on the decrypted, I just used the pointer to the node with value `0`. Then I rotated the list by the specified amounts, always starting from `0`. Since the rotation is smaller than the list's length, it is not necessary to take the modulo. But if it was, here it would be the modulo of the rotation by the list's length (because this time it is the whole list that is being rotated).

**Solution:** [day_20.c](./day_20.c) (finishes in *702 ms* on my old laptop, when compiled with `-O3`)

On a final note, if you are having trouble understanding the value's rotation, I recommend checking the answers to [this question](https://old.reddit.com/r/adventofcode/comments/zqh14z/2022_day_20_part_1_why_does_2_move_like_this/).