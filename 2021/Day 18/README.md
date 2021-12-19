## Explanation of the solution

I went for the elegant solution and made Snailfish Numbers into a class, and implemented their properties into the class. This way I can just sum two instances using the `+` operator and get the result. The class also has a method for the magnitude.

What I did was to represent the numbers in a flat array. Each element of the array held a few properties: the value, the position of the value in the node, the path to the node, and the depth of the node. By representing the numbers this way I can easily get the value of the adjacent values, while still keeping the information about about the structure of the number.

In order to sum two numbers:

* I concatenated their arrays
* Prepended [0] to the path of the first number's values
* Prepended [1] to the path of the second number's values
* Added 1 to the depth of all values

After the sum, I performed the reduction. There was a reduction loop, it first tried to check for explosions. If no explosions were found, then it tried to check for splits, otherwise the loop restarted. If a split was found the loop also restarted, otherwise the loop ended.

Explosion checking:

* Loop through all pairs in the array
* Check if the pair belongs to the same node (their path are equal)
* Add the right value to the next value to the right of the pair
* Add the left value to the next value to the left of the pair
* Get the position of the pair in its parent node (the last value in the path)
* Delete the pair from the array
* On its position insert the value of zero (position = position of pair in its parent; path = path of pair until second to last value)

Split checking:

* Looped through all values in the array for any >= 10
* Calculated the values of the new pair (divided value by two, and took the `floor()` and `ceil()`, respectively)
* The depth of the new pair is depth of old value plus 1.
* The path of the new pair is obtained by appending the position of the old value to its own path.

It is possible to calculate the magnitude from the array, without needing to recursively traverse the tree structure:

* Get the full path to the value (node path with the node position appended)
* For each [0] in the path, assign the multiplier 3
* For each [1] in the path, assign the multiplier 2
* Multiply all those multipliers to get the total multiplier for the value
* Multiply each value by its multiplier, then sum all results

It was quite challenging for me to build the class, but after finished it "just works". I can add any two values I want, and it just returns the sum and magnitude. The data structure I came up with is quite elegant and efficient!

Part 1 finished in 0.5 seconds for adding 100 values, while Part 2 finished in 6 seconds (because I needed to try all permutations of two values). Python's `itertools` module made easy to permute the values.

**Code:** [Parts 1 &amp; 2](https://github.com/tbpaolini/Advent-of-Code/blob/32689b5cde02941c9f65e881c32be5fb7404c351/2021/Day%2018/snailfish.py) (code is commented and with type annotations)

**Bonus:** [Helper function to build the binary tree](https://github.com/tbpaolini/Advent-of-Code/blob/c7099e5797a752de3fbc471dcbd7df4b1c73ee7d/Tools/binary_tree.py)
