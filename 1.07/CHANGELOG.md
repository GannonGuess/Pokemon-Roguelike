## Version 1.07

11/05 05:17 - Added stats class

11/05 05:09 - Added pokemon_stats class

11/05 05:00 - Added type_names class

11/05 04:48 - Added experience class

11/05 04:37 - Added pokemon_species class

11/05 04:19 - Added class and printing for pokemon_moves

11/05 04:00 - Added classes for storage of pokemon and moves
            - Pokemon and moves data is output after storage.

11/05 03:00 - Added database to computer and added checks for 3 different locations of database
            - If user looks to parse a file that is not in database csv folder, they get an error

11/05 02:40 - Check user input for which file they want to parse

11/05 02:30 - Copied instructor 1.06 over

## Version 1.06

10/29 15:24 - Packaged 1.06 for submission

10/29 15:09 - Added some comments and removed unneeded testing code
            - Invalid commands no longer print error to avoid weird outputs.
            - Buildings now have white background to make them pop more

10/29 14:00 - Map prints every time NPCs finish moving to make it more sclear where they are

10/29 13:38 - changed location of pathfinding call
            - fixed pathfinding bug (I think)
            - removed easter eggs

10/29 03:24 - Finished implementing flight movement
            - Error messages pop up when invalid flight input
            - Cannot fly to map you are currently in
            - PC is placed on a random path tile in the room it flies to

10/29 02:12 - Reimplemented movement between rooms
            - all rooms given their own heap queue
            - PC next move time is preserved based on room
            - PC no longer duplicates if entering room from a different enterance than it was exited from
            - Room location and player time are now printed below the map

10/28 23:21 - Changed all calls to malloc to new and all free to delete
                - Get experience with new options
            - Changed makefile to link C++ compiler
                - Code appears to be working

10/28 21:37 - Converted from .c to .cpp files
            - Changed all `structs` to `classes`
                - Moved around pc, npc, and character classes to allow for this
                - PC and NPC now inherit from character
            - made all variables public for now
            - Added greater than `>` and less than `<` for entering and exiting buildings

10/28 21:20 - Copied over files from 1.05


## Version 1.05

10/21 21:35 - Packaged 1.05 for submission

10/22 21:32 - Updated README and CHANGELOG
            - commented code
            - deleted unneeded code
            - Trainer list now only scrolls if not all trainers listed
            - NPCs should no longer trigger battles off each other

10/22 15:34 - Added error messages based off certain actions
            - updated `q` to `Q` for quitting game
            - Added --numtrainers switch to instructor code
            - Changed PC color to make it stand out more

10/22 07:31 - Trainer list now works
                - lists trainer types and relative positions
                - scrolling works

10/22 06:22 - Added easter egg to game and removed some experiments
            - PC can now walk on defeated NPCs and PC dispaly takes priority

10/22 04:12 - Fixed bug with pacer
            - All NPCs should now be able to tigger battle by moving into PC
            - Fixed segmentation fault near border by keeping gate flush with map border.

10/22 01:30 - Certain NPCs can now trigger battle by moving into PC

10/22 00:25 - Added color to game using ncurses

10/21 01:30 - PC can now start battles by moving into a tile with an NPC
            - Upon leaving battle, NPC is flagged as defeated

10/20 20:55 - Added NPC defeat flag and basic battle screen
            - Fixed bugged pacer movement

10/20 19:37 - Added checks for standing on buildings
            - Added screen pop-ups for buildings
            - Added trainer list popup
                - still need to populate
            - `q` no longer exits program with exit() but now just leaves loop and activates delete_world();

10/20 17:22 - Finished mapping movemnet commands to actully moving the PC
            - PC time updates accordingly
            - PC cannot move into NPCs or impassable terrain

10/20 16:23 - Added ncurses and command checking to ensure valid command inputs
            - Ncurses terminal initilization
            - Removed sleep command
            - Print map after every valid movement command
                - If command is not a movement, dont update map

10/19 22:09 - After failing to integrate new code with old, copied over instructor code

10/17 17:30 - Copied files for 1.05

## Version 1.04

10/08 21:00 - Packaged 1.04 for submission

10/08 20:45 - cleaned up code, commented, and updated README for 1.04

10/08 17:22 - completely rewrote how movement works so that actors do not get stuck near impassable terrain
            - prevent actors from gravitating towards the top of the screen

10/08 16:30 - reinsert nodes to that everything will update multiple times
            - segmentation fault, looking into movement function

10/08 15:42 - move() function in trainer.c takes input of removed heap node and moves it
            - updates actorMap with nodes new location and updates the actor's cost for its reinsertion
            - prints the screen if the selected actor is the player actor
            - sleep after printing

10/08 12:32 - updated trainer.c to use a priority queue for obtaining actor with shortest time
            - time_cmp() function used to determine which of two times is shorter, if the same uses shorter seqNumber
            - Actor copies made from actorMap and inserted into heap (could maybe cut out copies in the future?)

10/08 11:01 - finally fixed stupid error that prevented room struct from being passed into actor generation properly

10/08 01:30 - Added debugging comments and update user switch
            - Dijkstra now returns a distanceMap struct containing an array storing distances

10/08 00:15 - created trainer.c which generates trainers and adds them to actormap

10/07 23:32 - added trainer.h and structs for actors and actormap
            - add initilizer for actormap
            - removed redundant room_output function

10/05 09:23 - Changed player struct to no longer store the tile its on top of
            - Trees now represented as `4`
            - Created switch for calling file with specified number of trainers

10/01 21:20 - Copied files for 1.04

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