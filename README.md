# railway-maze-solver
An Implementation of Dijkstra's algorithm to solve train mazes with specific path restrictions.

This program was written within the context of a sophmore level data structures and algorithms class.  This program should be compiled using the -O9 compiler flag for a runtime of 10 seconds or less.  Without the passenger pickup restrictions, it will run in a fraction of a second.

# Railway Maze
A railway maze, invented by mathematician Roger Penrose, is a special kind of maze intended to emulate the path of a train.  The goal is to solve the maze in one continuous path, without reversing, and only making gradual turns (angles less than some constant value).  This program attempts to solve a train maze in the form of a .PPM file, using constraints defined in config.txt.  This problem also contains the additional constraint of having to pick up a number of passengers located at various coordinates along the way.

Here is an example of a valid solution to an example railway maze (including the additional constraints of passenger pickups noted in the following section):
![Railway Maze](https://github.com/mbpautz762/railway-maze-solver/blob/main/solution-example.jpg?raw=true)

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

My solution involves calculating and visiting neighbor nodes "on the fly" as opposed to pre-calculating the entire graph in order to reduce running time.  A neighbor is considered valid if:
  • the RGB values at the coordinate in question indicate it is a valid path pixel, as defined in config.txt.
  • it falls within the distance constraints outlined in config.txt.
  • it's angle (calculated using vector dot products from it's previously-visited node) falls within the angle constraints outlined in config.txt.
  • it lies in the same direction, as determined by it's previous node.
  
Each valid neighbor for the node in question is added into a vector and visited in order, using Dijkstra's algorithm and a priority queue.  Predecessor nodes are tracked, so when a solution is found, a path can be drawn back to the start.

# Passenger Pickups

  passenger pickups are tracked using a short int.  When a passenger's node (outlined in config.txt) is visited, a set value is added to the short.  A solution is only found when the end node is found AND the short int contains the proper value to indicate the specific passenger have been picked up.  Without this passenger restriction, this program finds a valid path much faster.
  
  This readme is a work in progress and will be updated as the project is updated.
