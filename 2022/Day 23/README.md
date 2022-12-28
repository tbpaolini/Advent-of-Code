## [Day 23 of 2022](https://adventofcode.com/2022/day/23)

In order to represent the elves' coordinates, I used a hash table. The reason for that was that I did not know at first how big the map would become, specially considering that sometimes a Part 2 is Part 1 with ridiculously big numbers. Using an 2D array for the coordinates should be a bit faster than a hash table, but an 2-D array begins using too much memory as the dimensions of the map grow.

Since I am using C with only its standard library, it was necessary to code a hash table. But that is a good thing for me because the reason I chose C this year is to train writing data structures and algorithms.

A hash table is a structure that associates a key to a value. In this case, the key is the coordinate and the value is the count of elves on that position. The main feature of a hash table is that, if implemented correctly, should take roughly the same amount of time to retrieve a value, regardless of the amount of keys. That is because the index where a value is stored is calculated by a hash of the key, and the value will be always the same if the key is the same.

Since the table has way [less spaces than the amount of possible keys](https://en.wikipedia.org/wiki/Pigeonhole_principle), there will be keys that will be mapped to the same index. This is called collision, and there are different strategies to handle that, depending on what you want to optimize. I used "separate chaining", which means to have a linked list on each index of the table. New values are appended to the list.

This method has the advantage of being easy to implement and of enabling the table to store an unlimited amount of values. But it also has the disadvantage of having the data scattered all around the memory (bad for the CPU's cache), and needing to resolve some pointers (which is slower than reading the data directly). I mitigated that by having the first node of the list stored directly on the table. Most of the values will be stored on the first nodes anyways, if the table is big enough and the hash function has good dispersion.

The hash function that I used was the [FNV-1a](https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function). It is known for being very fast, easy to implement, and having a good dispersion. It is worth noting that this hash is not cryptographically safe, meaning that it is possible to intentionally craft keys that yield the same hash than another. But that should not be a problem in our case.

The way I implemented the hash table behaves similarly to [Python's Counter](https://docs.python.org/3/library/collections.html#collections.Counter). When a new key is added, its counter is set to 1. And when the same key is added again, its counter is incremented by 1. If a key is not present on the table, its counter is considered to be zero.

I had a main hash table for storing the current coordinates of the elves. I made a function that dumps all the stored coordinates into an array. For each movement round, I dumped the current coordinates of the elves and then I iterated over them. For each coordinate, I checked the count of elves on the 8 neighboring cordinates. If the movement conditions were met, I then added the movement to a queue. I also a secondary hash table for storing how many elves were considering to move to.

Then I iterated over the queue in order to check if the movement was possible: I checked on the secondary hash table for how many elves were trying to move to the position. The movement was performed only if a single elf was trying to move there. In order to perform a movement, the old coordinate was removed from the main hash table, then the new coordinate was added to it. The secondary hash table was resetted at the end of the round.

For calculating the free space after 10 rounds, I dumped the coordinates, then I checked for the minimum and maximum coordinates in order to calculate the dimensions of the smallest rectangle that contained all elves. Then I subtracted the rectangle's area by the amount of elves there. In order to determine when the elves stopped moving, I had a flag that was set to `true` when any elf had moved in a round. Then I just kept performing rounds until no elf has moved.

**Solution:** [day_23.c](./day_23.c) (finishes both parts in *1.82 s* on my old laptop, when compiled with `-O3`)

(Part 1 alone finished in *31 ms*)