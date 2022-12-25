## [Day 22 of 2022](https://adventofcode.com/2022/day/22)

This took way longer than I expected, because visualizing an object in a 3D space is not my strong point. But I managed to get a [3D interactive cube](https://tbpaolini.github.io/Advent-of-Code/2022/22/cube.html) working in order to help me to see how the faces connected. I did not write the code for the visualization, I just numbered the cube faces. The 3D cube was coded by Jordan Leigh ([source](https://codepen.io/jordizle/pen/bGReWV)). From that, I drew a [diagram showing how the faces connect](https://github.com/tbpaolini/Advent-of-Code/raw/master/2022/Day%2022/Cube%20diagram/cube.drawio.png).

For solving the puzzle, I used a graph to represent the board. Each node on the graph has exits for Part 1 and for Part 2. For the later, specifically, there are also values telling how the direction changes depending on which exit is taken (to represent walking around the cube).

I parsed the input file line by line and I stored the node values on an temporary array. A value was considered to be on a map border if it had a blank space next to it, or if it was on the border of the array. After the input was parsed, I iterated over the array in order to link the nodes, then I checked each of the node's 4 exits.

For Part 1, if the exit was not at a border then the node was just linked to the coordinate next to it on the exit's direction (if there was no wall there). If the exit was at a border, then I used a different logic for each part in order to find the exit. I kept moving on the opposite direction of the exit until there was an blank space on the next spot, or until the array border was reached; then I linked to that coordinate if there was no wall.

For Part 2, I hardcoded the cube's connections, since the shape is always the same for all the inputs. I divided the map in quadrants of 50 by 50, each representing a face of the cube. Then I calculated the (x, y) coordinates within the face. If the exit was not at a face edge, then it was just linked to the coordinate next to it if there was no wall. If the exit was at a face edge, then I checked which face it connected to, and I linked to it if it had no wall.

This was the toughest thing to visualize. It is important to pay attention if the edge changed the vertical or horizontal orientation. If it did, then the face's (x, y) coordinates are swapped when wrapping to the other face. It is also necessary to see if the direction of the axis changed (the direction in which it increases or decreases). If so, then the face's coordinate on that axis is mirrored along the middle of the face. It was for me as tricky as it sounds, it took a while for me to get it right, I needed to cross check with [this solution](https://reddit.com/r/adventofcode/comments/zsct8w/2022_day_22_solutions/j1jzg4z/) in order to be sure I got it right.

I also stored on the graph the arriving direction when you moved through an exit at an edge. Since you can only arrive at a face from one direction, it was sufficient to just store the absolute direction on the node's exit.

After all the nodes were linked on the graph, traversing it was the easiest part, because all the checks were made while building the graph. It was just necessary to remember on Part 2 if the exit changed the direction.

**My solution:** [day_22.c](./day_22.c) (finishes in *9 ms* on my old laptop, when compiled with `-O3`)

On a final note, since my solution hardcoded the cube's shape, here is [a solution for any shape](https://reddit.com/r/adventofcode/comments/zsct8w/2022_day_22_solutions/j1fulrk/).