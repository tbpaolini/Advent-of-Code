## [Day 21 of 2022](https://adventofcode.com/2022/day/21)

I used an array for storing the monkeys' data, and a [trie](https://en.wikipedia.org/wiki/Trie) (preffix tree) for searching the monkeys.

This trie associates the monkey's name with the pointer to the monkey's data. Each node of the trie has 26 branches, one for each of the `a` to `z` letters. The root node (depth 0) represents the first letter of the name, the depth below (depth 1) represents the second letter, and so on. Starting from the root node, in order to store a name, I took the branch corresponding to each character. On the node where I ended on, the monkey's pointer was stored.

For retrieving a name, the same process to arrive to the node where the pointer is stored. Since all names have four letters, we need to always traverse through four nodes in order to find a monkey. This is faster than doing a linear search through the 1605 monkeys, and simpler than using a hash table (perhaps even faster, because hashing may take a considerable time).

I parsed the monkey's data into the array, and stored each monkey's pointer on the trie. If the first character on the input after the `: ` was a digit, then the monkey was considered a "number monkey", and the string was converted to a **double-precision floating point number**. Otherwise, it was an "operation monkey", the string was split in the spaces, and the operation and other monkey's names were stored.

After that, I looped through the array in order to link each monkey to the other monkeys they were waiting for. The trie was used for retrieving the pointers from the names. This prevents us from needing to do a search every time that an operation needs to be made, saving some time.

For getting the number that a monkey should yell, I used a recursive function. If the monkey is a "number monkey", then the number is returned. If it is an "operation monkey", the function calls itself on the monkeys that are being waited for, then it performs the operation on the other monkey's numbers, and returns the result. Deciding which operation to perform was accomplished through a `switch` statement over the operation's character.

Part 1 is just calling the function on the `root` monkey. Part 2 requires us to find which number `humn` should yell in order for both values waited by `root` to be equal to each other. This means that on Part 2 `root` should perform a subtraction. If the result is zero, then the two values are equal.

The way I used for finding `humn`'s number was to use the [gradient descent](https://en.wikipedia.org/wiki/Gradient_descent) algorithm in order to make `root`'s number to converge to zero while changing `humn`'s number. Basically, we make a couple of initial guesses for the number, then we change the guessed number in the direction that the function decreased (and proportionally by the amount it decreased).

The gradient of a function is the direction and intensity in which the function is increasing. For a function with a single variable, the gradient is just the function's first devivative (the slope at a point on the function's curve). The derivative can be calculated by dividing the difference of two results by the difference of their inputs (the guessed numbers).

After each guess, I calculated an error function: the absolute value of the `root`'s current number, that is, how far the number is from zero. The guesses continued until the error was smaller than `0.1`. Then the `humn`'s number was rounded to the nearest integer, which was the solution for Part 2.

**Solution:** [day_21.c](./day_21.c) (finishes in *10 ms* on my old laptop, when compiled with `-O3`)

### Python 3.10 (no *eval* )

I used Python to test and tweak the solution that I implemented in C. The obvious choice for solving this puzzle in Python would be using its `eval()` function to parse the operation. But since this is not an option in C, I went through a different route. Also it is worth noting that `eval()` can be a serious security risk, because it allows others to execute arbitrary code in your program.

I stored all monkeys in a dictionary. If it was a "number monkey", I just converted the string to integer and then stored the value.

I used the [operator library](https://docs.python.org/3.10/library/operator.html) in order to import the operations that the puzzle uses. This library allows you to import functions that perform the operations of the Python's operators. It is worth noting that I am using here the floating point division (`truediv()`), rather than the integer division (`floordiv()`). Then if the monkey did an operation, I splitted the operation string, and I stored the monkeys' names and a reference to the function that should be applied to those monkeys' numbers.

I used a recursive function to calculate which number a monkey should yell. For a number monkey, the function just returns its number. For an operation monkey, the function calls itself on each of the monkeys that are operated on, then performs the operation on the returned values, and returns the result.

For Part 2, I changed the operation of the `root` monkey to subtraction. If it returns zero, then that means that both operands are equal. Then the goal was to converge the result to zero. In order to do that, I had an error function that is just the absolute value that `root` returns (how far from zero the result is). Then I used the [gradient descent](https://en.wikipedia.org/wiki/Gradient_descent) algorithm for minimizing the error function.

Basically, on each iteration the guess is changed in the same direction and proportion that the error changed from the previous guess. The process continues until the error is below a certain threshold, which I chose to be `0.1`. Then the last guess was rounded to the nearest integer, which was the solution for Part 2.

**Solution:** [day_21.py](https://github.com/tbpaolini/Advent-of-Code/blob/master/2022/Day%2021/day_21.py) (finishes in *161 ms* on my old laptop)