## Advent of Code: Day 1 of 2023

For the **Part 1**, I just read the string from left to right. I kept integer variables for the first and last digit, and also a flag telling whether the first digit has been parsed already. Then I updated those variables when a digit was found. At the end of the string, I incremented the solution by `(10 * first_digit) + last_digit`.

A potential issue that can come in this part is not realizing that some strings might contain only one value, in which case that value is both the first and last digit. So the last digit is always updated when looping through the string.

For **Part 2**, I used a [trie](https://en.wikipedia.org/wiki/Trie) for checking if a substring matched a digit's name. In order to keep track of of the substring I used a "two pointers" approach: one to the `start` and another to the `end` of the substring (inclusive). Both begin at the first character, and while the substring is contained in the trie, the `end` pointer was moved to the next character. If `end` reached a perfect match for a digit's name, then the digit's value was outputted and both `start` and `end` were set to the next character where the substring started. If `end` reached a digit character, then its value was outputted and both `start` and `end` were moved to the next character after the digit.

A potential problem on this part is that some digit's name overlap. For example, "one" ends in "e", which is also the first letter of "eight". That is why when matching a name the `start` pointer was moved to the next character, rather than to the end of the substring. This way overlapped names are not missed. The downside of this approach is that some parts of the string might be scanned more than once. If you would like to avoid that, you might want to have a look at the *[Aho-Corasick algorithm](https://cp-algorithms.com/string/aho_corasick.html)* (which is more complex to implement, but it can find multiple keywords with just a single pass on the string).

All in all, this problem ended up being much harder than what I anticipated for a Day 1 puzzle. But at least I could learn more about searching on strings :)

**My solution:** [day_01.c](https://github.com/tbpaolini/Advent-of-Code/blob/master/2023/Day%2001/day_01.c)