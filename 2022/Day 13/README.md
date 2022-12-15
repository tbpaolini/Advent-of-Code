## [Day 13 of 2022](https://adventofcode.com/2022/day/13)

Since C does not have some kind of an `eval()` function to parse an array literal, it is necessary to make a parser from scratch. This sort of scenario is why I chose C this year, in order to increase the challenge and learn something different along the way. :-)

In order to represent the nested lists, I used an struct with an array of pointers to the values and the amount of elements on that array. Each element points to a struct, which contains an union that can store either an integer directly or a pointer to a nested packet. The struct also has a flag that tells whether the value should be read as a packet pointer or an integer. When the depth reached `-1` again, then that meant the parsing was done.

For parsing the packet, I iterated over the characters on the line, while keeping track of the current depth on the nested list. The depth started at `-1`, it increased by one when a `[` was found on a string, and decreased by one when a `]` was found. A `[` also meant that a nested packet was found, and a digit meant that an integer was found. If the current depth was `0` (the top level), for integers they were added directly to the packet, while for nested packets the parsing function was recursively called on them.

In order to compare packets, I iterated over the values of both packets at the same time, until at least one packet ran out of values. If both values were integers, they were compared directly. If both were lists, the comparison function was recursively called on them. If one was an integer and the other a list, the integer was converted to a list of one element, and compared with the other list by also calling the function recursively.

Sorting the packets was done with the `qsort()` function of C's standard library. That function takes took a pointer to my custom comparison function, an array of packets, and it sorted the array in-place. Before sorting, the divider packets were added to the packet array.

**Solution:** [day_13.c](./day_13.c) (finishes in *7 ms* on my old laptop, when compiled with `-O3`)