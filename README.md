# train-maze-solver
An Implementation of Dijkstra's algorithm to solve train mazes with specific path restrictions.

This program was written within the context of a sophmore level data structures and algorithms class.  This program should be compiled using the -O9 compiler flag for a runtime of 10 seconds or less.

# Railway Maze
A railway maze, invented by mathematician Roger Penrose, is a special kind of maze intended to emulate the path of a train.  The goal is to solve the maze in one continuous path, without reversing, and only making gradual turns (angles less than some constant value).  This program attempts to solve a train maze in the form of a .PPM file, using constraints defined in config.txt.  This problem also contains the additional constraint of having to pick up a number of passengers located at various coordinates along the way.

# Problem Constraints
In it's current form, the config.txt file contains the following information: 

• The name of the PPM image describing the maze to be solve.

• (x; y) coordinates of the starting point pixel (x is horizontal and y vertical, with (0; 0) being
the upper-left corner).

• (x; y) coordinates of the goal point pixel.

• The next four lines respectively give (x; y) coordinates of 4 passengers that need to be picked up.  In it's current state, these names are Dean, Lin, Perez, and Reeder.

• The threshold T governing which pixels are valid to choose as part of the solution path.  a pixel is part of the path if the sum of its red, green, and blue color values is at most T.

• The minimum and maximum Euclidean distance each step of your path can take. This ensures that each step taken is not too short or not too long.

• The maximum number of degrees M you can turn left or right in any step of the path relative
to the direction of the previous step. This constraint is what forces you to only be able to
turn gradually.

• A distance threshold D specifying the minimum distance you must be from one of the four
passengers above before you can pick them up. This distance threshold also governs the end of the path.

# Solution
A solution is found when the 'train' reaches the destination under the preceeding constraints AND contains passengers named Dean, Lin, and EITHER Perez or Reeder.  In the coming weeks as I improve this program, I will make it possible to define your own passenger constraints in the config.txt.
