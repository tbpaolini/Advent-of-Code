## [Day 7 of 2022](https://adventofcode.com/2022/day/7)

This puzzle is where a more elaborate data structure is necessary, and knowing recursion might help too. :-)

I used a tree in order to represent the file structure. Nodes can represent either a file or a directory, they store the name and the size. The directory nodes have a linked list with their child nodes. Each node has a pointer to their parent directory. To simplify things, I also stored the counter of immediate subdirectories for each directory.

I parsed the commands through a series of if/else statements. I am aware that this wouldn't scale well for a larger amount of commands, but it suffices for what we have. If the first character of the line is `$`, then it is a command, which can be either `cd` or `ls`. Those instructions were used to navigate through the tree.

If the line is not a command, then it is a directory listing. In this case, if it starts with `dir`, then it is a subdirectory. Otherwise, it is a file. Then the data was parsed accordingly and added to the tree. If it was a file, its size was stored at this point.

The size of the directories was calculated after all the input was parsed. I used a recursive function that sums the size of all the filles in the directory. And when a subdirectory is found, then the function is called on it. After that, I recursively searched the tree for the sizes of all directories in it. The search function starts from the root node, and get the size of the files in it. If a subdirectory is found, the function is recursively called on it.

Those size values were put in an array. Then I iterated over the values to find all sizes below `100000`, and also find the smallest size to be deleted in order to free enough space.

**Solution:** [day_07.c](./day_07.c)