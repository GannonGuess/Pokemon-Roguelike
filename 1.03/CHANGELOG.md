## Version 1.03

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