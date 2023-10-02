## Version 1.04


## Version 1.03

10/01 21:10 - Packaged 1.03 for submission

10/01 18:23 - cleaned up code
            - commented code for 1.03
            - Updated README for 1.03

9/27 19:21 - added printing of both hiker and rival maps. Both appear to be correct!

9/27 17:42 - added checks for skipping unpassable terrain so costs are not updated
           - nodes should now only be added to heap if cost is updated
           - does not update node if new cost would exceed infinity (is negative)

9/24 19:33 - Rewrote how dijkstra algorithm checks neighbors to be inside of a for-loop rather than having repeat code 8 times
                - Easier to add changes in one place rather than 8

9/23 13:55 - rewrote instructor dijkstra to try and get it to work with creating a cost map. currently not working
           - sometimes cost exceeds infinity and overflows
           - Added printing of cost maps

9/24 12:07 - Added function for obtaining the costs of tiles based on npc type and tile type

9/23 23:22 - copied over heap.c and heap.h from instructor 1.02 solution
           - copied dijkstra algorithm used in instructor 1.02 path creation into dijkstra.c
           - removed path creation elements and weighting

9/23 20:41 - created dijkstra.c and dijkstra.h for creating cost maps

9/18 20:24 - Added placing player at random map location and colorization of map elements

9/18 11:17 - Added player struct for displaying PC location and storing its information

9/17 22:41 - Added skeleton code for potential colorization of map

9/17 21:54 - Copied files for 1.03

## Version 1.02

9/17 21:30 - Packaged 1.02 for submission

9/17 20:34 - Comments added and README updated

9/14 12:13 - Paths should now no longer turn at border, so they no longer look as bad
           - hasMart and hasCenter now init properly
           - printing map gives its room location

9/13 10:28 - Added "flight" movement method to quickly move to selected room. Error displayed if room cannot exist
             or wrong input format

9/13 10:04 - Paths should now no longer exit rooms on world border

9/12 21:27 - Finally fixed world malloc(). Used to create world_map with filled rooms causing hours of headache

9/11 11:39 - Added Manhattan distances for each room. Centers and Marts will now be generated based on Manhattan
             distance.

9/11 10:41 - Added commands for navigating between rooms. Error message displayed if wrong format. Still need flight

9/11 10:23 - Redesigned path creation so that rooms align. Still need to fix for corner cases

9/11 07:55 - Creating rooms as they are entered, saving them to an array of rooms

9/10 22:08 - Creating struct for containing world

## Version 1.01

9/10 21:30 - Packaged 1.01 for submission

9/10 21:15 - Added boulder generation for variety

9/10 21:09 - Added tree generation for more variety

9/10 20:39 - Altered terrain generation to make bigger patches of terrain more often

9/10 19:33 - Added Center and Mart generation that connects them to the nearest path with a 2-wide path

9/10 15:32 - Added path generation, choosing random turn points

9/10 14:23 - Added variable terrain generation with structs (water and tall grass)

9/10 13:17 - Added structs for defining rooms. Redefined initial border and short grass generation to use the struct

8/29 13:15 - Added enterance and exits in cardinal directions for paths to connect

8/29 12:30 - Added basic random terrain generation for water and tall grass

8/29 12:07 - Adding grid initilization and terrain generation without structs (short grass and borders)

8/28 19:42 - Created README, Makefile, terrain.c, and CHANGELOG