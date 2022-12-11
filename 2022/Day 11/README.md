## [Day 11 of 2022](https://adventofcode.com/2022/day/11)

Before anything else, let's talk about the elephant in the room: the meaning of Part 2's "*ridiculous levels*" of worry, and how to "*find another way to keep your worry levels manageable*". That is a cryptic hint telling you that the **integers are likely to overflow** on this part. This breaks the division test if you are not in a language that supports arbitrary integer sizes. And even if you are, the values will be so big that the program will slow to a crawl.

In order to prevent that, it is necessary to take the modulo of the worry level by a common multiple of the test divisors. That should be done before the division test. This does not change the result of the test because the modulo operation wraps back to zero when the dividend is multiple of the divisor. Ideally your should be the least common multiple (LCM) among all divisors, but any common multiple will do as long it does not overflow too. Since all the divisors in this puzzle are primes, it should suffice to just multiply then to get their LCM. I would be lying if I said that I realized that myself, I had to look for hints. But learning is part of the process :)

Now with the elephant out of the way, let's head back to the beginning. The first challenge is to parse the data from the input file. Since all values of the same category always begin on the same position on the line, I just read the values from those positions. I also made assertions to check if the rest of the line was what I expected to be.

I used a struct to store the parameters of each monkey: the operation they should do, the value they should add or multiply, the value for the division test, which monkeys to throw depending on the test outcome, and the counter of how many times the monkey inspected an item. It is worthy noting that for the value to be summed or multiplied, I used the maximum 64-bit unsigned integer in order to represent that the current worry level should be used as the value.

The monkey struct also had the list of items currently with the monkey. I used an queue for that: items are added to the end of the queue, and removed from the beginning. I implemented that as doubly linked list.

Once all data is stored properly, it is not difficult to perform a round:

* Starting from the first monkey, pop the first item of the queue.
* Perform the operations on the item's worry level (do not forget to modulo by the LCM).
* Test which other monkey to pass the item.
* Append the item to the queue of that monkey.
* Repeat until there are no more items, then go to the next monkey.

The logic does not really change between parts 1 and 2, the catch is that you need not realize that an overflow might happen in Part 2, and give the wrong result even though the logic is technically correct.

**Solution:** [day_11.c](./day_11.c)