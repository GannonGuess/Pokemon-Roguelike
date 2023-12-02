# 327 Pokemon Game README - Gannon Guess

## Version 1.10



## Version 1.09
This verion adds the pokemon battle minigame, trainer battles, wild pokemon battles, and bag mechanics.
- Using the instructor code lost me the ability to display the PC's party in the overworld map. I did not readd this to this version as it has never been needed, but plan on adding it for version 1.10. 
- `NOTE:` Because battle-loss behavior was left unspecified in the instructions for this version, I am planning on implementing it the next version. In it, I will be sending the PC to the origin map's pokemon center. For now, the PC remains where it lost and the pokemon center screen is activated to heal all pokemon. The PC will therefore continue to battle the NPC that they just faced until the PC wins or teleports away with `p`. Using `p` to teleport will have the NPC try to battle the player one last time. Finishing this battle should place the PC in their new, teleported to, location. I figured the new functionality would be good for the final assignment.
- All pokemon battles give the PC an option of `FIGHT`, `POKEMON`, `BAG`, or `RUN/FLEE` on their turn.
  - These menus are navigated with the `UP` and `DOWN` arrow keys, with a `*` to denote the currently selected item.
    - All battle menus use a similar style of selection
  - The `RETURN` or `ENTER` key is used to verify the users selection. `ESC` can be used to go back
- The `FIGHT` option lists the current active pokemon's moves alongside the moves' `POWER` (POW) and `ACCURACY` (ACC) values
  - Selecting a move with `0 POWER` does the minimal amount of damage a move can do (usually 2)
  - Selecting a move with `0 ACCURACY` means that the move will miss
    - This means that you could get unlucky with a starting pokemon that can never damage enemies. Very sad, but not something I need to account for. Thats just how those moves work, even in the Main Series Games.
- The `POKEMON` option lists all of the PC's pokemon and their current HP. The user can select a pokemon to swap out to
  - Swapping pokemon uses the PC's turn just like in the Main Series Games (MSG)
  - PC cannot swap to a pokemon that has fainted or to their currrently active pokemon
- The `BAG` option lists the items in the PC's bag and a menu to select which one to use.
  - The number of items remaining of a particular type are listed alongside the item names, as well as a description of what the item does
  - Attempting to use an item that the PC has 0 of does nothing
  - Potions can only be used on pokemon that have taken damage and have more than 0 HP
  - Revives can only be used on fainted pokemon (0 HP)
  - Pokeballs will always capture wild pokemon, and pokeballs used in trainer battles will be lost and do nothing just like in the MSGs.
    - Pokeball will fail to capture a wild pokemon if the PC has 6 pokemon already and the pokmeon will flee.
  - `ESC` can be used to back out of the bag
  - The `BAG` can also be accessed with the `B` command outside of battle in the overworld. Here, pokemon can be healed or revived, but pokeballs do nothing. `ESC` or using an item exits the bag without consuming a turn.
  - Using an item during a battle uses the PC's turn.
- The `RUN/FLEE` option does not work in trainer battles. In wild pkmn encounters, the trainer has 66% chance of getting away safely
  - Wild pokemon also have a 10% chance of fleeing during their turn.
  - Failing to flee a wild pkmn encounter uses the PC's turn
- Visiting a PKM Center will heal all of the PC's pokemon to full health
- Visiting a Pokemart will restock all of the PC's items
- All PC pokemon fainting during trainer battle will result in the end of the battle. The PC and enemy trainer remain in their current positions, and all PC and enemy trainer pokemon are healed for rematch.
- I changed damage calculations slightly from the specified instructions since low level pokemon would deal thousands of damage to each other with the given formula. I balanced the damage by drawing from the calculations of Generation 1 from the main series games. The new calculation starts with low damage at low levels (usually 2-4), but prevents all attacks from one-shotting everything at high levels. I found this to be more enjoyable to play and much more balanced. The variables used in damage calculation remain the same, but some round down occurs to keep values from being too high and an extra instance of integer division is used.
  - With the way attack and defence are calculated, higher level pokemon deal less damage to pokemon of a similar level, and more damage to lower level pokemon. The inverse is true for low level pokemon. They do less damage to high level pokemon.
- If an attack does more damage than the remaining HP of the defending pokemon, the attack amound displayed to the user is the remaining HP of the defender just like the MSGs.
- Enemy trainers will send out their pokemon in order, only switching when the active one faints just like in the MSGs. They will chose a random move for their currently active pokemon to use on their turn.
- During trainer battles, the enemy trainer pokemons' moves are displayed, but without POW or ACC stats. The number of pokemon they have is also displayed next to the name of their current pokemon.
- If a PC pokemon faints during battle, the pokemon selection screen is displayed if the PC has another pokemon to use. Otherwise the battle ends in a loss.
- If all of the enemy trainers pokemon faint during the battle, the PC wins and the NPC is set to the loss state added in a prev version.
- The pokemon with the higher move priority attacks first. If priority are equal for both pokemon moves, the pokemon with more speed goes first
- A move only hits and deals damage if an accuracy check is passed. If its not passed, the player is told that the move misses
- Pokemon HP display is changed after both trainers have made their move. 
- Enemy trainer PKMN are only healed if they win the battle. This is to allow for rematches like the MSGs
- Pokemon levels are displayed during battle.
- The PC using any command other than FIGHT causes the selected command to go first. (Ex. PC healing takes priority over NPC attack)


## Version 1.08
This version adds pokemon to the game, including selecting a starter, encountering pokemon in the tall grass, and seeing the pokemon that other trainers have.
- monster class created to hold pokemon information.
- character class now has an array of monsters of size 6 so that each trainer and the user can have up to 6 pokemon.
- All Pokemon are chosen at random from the database upon generation
- 3 lvl 1 starter pokemon are generated and presented to the player on start-up.
  - Starter pokemon is selected by user through associated number key.
- 15% chance of encountering ANY pokemon when walking into a tall grass tile.
- All trainers have at least 1 pokemon, and then a 60% of having n+1 additional pokemon when they are at n pokemon.
- Added a command `i` when on the map-view to view the list of pokemon the player has (currently limited to 1 as there is no battle or capture). Press any key to leave this view.
  - While on the pokemon list view, the `+` command levels up your pokemon by one level (up to 100) so you can see how stats are affected.
- The Pokemon stats that are shown to the player are calculated using a formula that takes inputs of "hidden" base stats (the player never sees these) from the database, generated IV values, and the pokemons current level.
  - IV values are a number between 0 and 15 inclusive that are calculated upon pokemon generation.
- The chosen starter pokemon stats are shown upon selecting the starter. They can be checked again at any time with `i`. Press any key to exit. Not all starter stats are shown to stay in line with MSGs.
- NPC Trainer pokemon stats are shown upon encountering an NPC trainer. Press any key to exit.
- Wild pokemon stats are shown upon encountering a wild pokemon. Press any key to exit.
- Pokemon gender is 50/50 male or female and is denoted by `M` for male and `F` for female.
- A pokemon has a 1/8192 chance of being shiny. Shiny pokemone currently have a `*` star after their name to denote their status
- Pokemon levels are generated according to the specified requirements. The further from the origin, the higher of level pokemon you will encounter. Only level 1's are found at the origin and only level 100's are found at the furthest points.
- Pokemon move ID's are found based on the pokemon species, and then the moves database is searched by move ID for the move. If no move exists for the pokemon at its current level, the pokemon is leveled up until it can have a move. Only on-level-up moves are allowed.
- If a pokemon only has one available move at its level, its move slot is left blank on output.

## Version 1.07
This version adds the pokedex database. The user cannot play the game, but instead chooses an option of what information from the database to store in a vector. That vector's information is then stored printed to the terminal.
- The command given by the user is passed in with the call to the game.
  - ex. `./poke327 pokemon` will print all of the pokemon in the pokedex and their associated information found in `pokemon.csv`
Commands include:
  - `pokemon`: gives all pokemon in pokedex and info such as weight, height, base exp, and id
  - `moves`: gives all moves that can be learned by a pokemon and associated info
  - `pokemon_moves`: information relating pokemon to the moves they can have
  - `pokemon_species`: information on the species of pokemon including evolution and biome data
  - `experience`: information on experience required for leve-ups
  - `type_names`: lists all type names in different languages - I opted to include non-english type names
  - `pokemon_stats`: defines stat values for each pokemon
  - `stats`: defines stat names and where they are used
  - `pokemon_types`:defines the types that each pokemon is
- `.csv` is appended automatically
- When a command is called, the associated file is located and opened. It is then parsed into a vector containing instances of the associated class. The instances stored in the vector are then printed out for the user to view. The vector is returned by the parse function.
- INT_MAX is stored in place of empty fields when parsing. When printing, INT_MAX is converted to print "" denoting an empty field.
- Printing also includes header field definitions at the top of the print
- Three directories are checked for the database csv storage:
  - `/share/cs327/pokedex/pokedex/data/csv/`: first checked, would need to be a part of this package which it cannot be due to submission rules
  - `$HOME/.poke327/pokedex/pokedex/data/csv/`: Checked if first fails. Should be what works for submission checking. Works on my end.
  - `$HOME/dev/pokedex/pokedex/data/csv/`: Final check if first two fail. A directory specific to my computer that works on my end.

## Version 1.06
In this version, I ported the game from C to C++. Classes and inheritance replace the old struct format, and the compiler was changed to compensate. Room/map traversal and the flight command were reimplemented with this version to allow world traversal.
- All .c files changed to .cpp
  - Compiler now links in C++ compiler
- Classes are now used in place of structs
  - PC and NPC inherit from Character class
- Calls to `malloc` are now calls to `new`
  - `free` is now `delete` to compensate
- `>` is now used to enter a building, making up for me mistaking it for `<right arrow>` in the last version
  - Similarly, `<` is now used to exit a building
- Map is now printed after each NPC moves, giving the Player a better idea of, and reaction to, NPC movement.
- Each room/map has its own PC/NPC movement queue to preserve movement order and times
  - Each room preserves the PC's movement time upon exit from the room
  - Upon reentering a room, PC's movement time is set to value it left with. This allows the PC and NPCs to move at the correct times since time does not pass when PC is not in a room.
    - This information is printed below the map for visualization
- Movement between rooms via gate places PC on tile abeam of the previous rooms exit gate.
- Movement between rooms via flight places PC on random path tile in destination room.
  - Landing on an NPC will rightfully start a battle
- Flight is started with `f` command and takes two integer values, separated by a space, in range of [-200 : 200]
  - Echo is turned on so that player can see the coordinates they are entering. It is turned off after flight or upon input error
  - First integer is X coordinate, second is Y coordinate.
  - Cannot fly to a room you are currently in. This prevents the PC from avoiding battles easily
  - Invalid input displays an error message and flight is canceled.
!!!PLEASE NOTE: Instructor mentioned fixing a pathfinding bug that causes an error. I had this issue at the start of the 1.06 assignment, since this version is built off of Instructor Version 1.04 as a base. I believe that I found and added his fix from 1.05 to my own code and have not encountered any errors in my testing since, but BE AWARE that pathfinding may very rarely cause a segmentation fault that I don't have time to test for right now, but I haven't encountered it recently when I ran around the map for maybe 5 minutes. This error was stated to be "not even your (the students) fault!". Going forward in 1.07 I will use the Instructor's 1.06 code to be even more sure that this error does not persist. Feel free to run this version with the debugger to be sure that if a segmentation fault occurs, it is due to pathfinding.

## Version 1.05
This version adds the user interface, allowing for unbuffered command inputs using the ncurses library. Whenever it is tha player's turn, the game will halt and wait for a command to be input. Due to incompatability with the structure of version 1.04, the instructors code was brought in for the base of 1.05. Hopefully this will not happen again in the future.
- Color added
  - Makes screen flicker on refresh sometimes. Not an issue overall though.
- PC movement commands added
  - `y/7` - up left
  - `k/8` - up
  - `u/9` - up right
  - `l/6` - right
  - `n/3` - down right
  - `j/2` - down
  - `b/1` - down left
  - `h/4` - left
  - `5/./<space>` - wait
  - `<right arrow>` - attempt to enter building
    - `<left arrow>` - exit building
  - `t` - display trainer list with relative positions
    - `<down arrow>` - scroll down trainer list
    - `<up arrow>` - scroll up trainer list
    - `esc` - return to character controls
  - `Q` - quit the game
    - NOTE lowercase `q` WILL NOT WORK. This was done to prevent accidental exit of game.
    - QUIT command does not work when in trainer list, when inside building, or when engaged in battle
- Error messages should be displayed if attempting to move into impassable terrain, a gate, or attempting to enter a building where there isn't one.
- If the PC attempts to move into a cell that an npc is currently in, a battle will begin
  - Due to how the time system works in this game and how the map is only updated after the PC moves, the NPC may seem to be two squares away when the battle starts. Their move was just calculated at a different time. Player must be aware of how movement works to avoid unwanted battles, adding some forethought.
  - Since neither the PC or NPC have pokemon, nothing can be done. Press `esc` to exit the battle and flag the NPC as defeated
- A battle will also trigger if the NPC attempts to move into the PC's cell during their turn
- Neither the NPC nor the PC will move if they triggered a battle on their turn. This is done so that the PC and NPC come out of battle with some space for the pokemon to have fought. Kind of hard to fit everyone on the same cell.
- After a trainer is defeated, they are set to sentry movement (standing still), just like the classic pokemon games. Guess they took the loss a little too hard. They cannot be battled again as you crushed their spirits.
- Defeated trainers can be walked over to prevent soft-locks in tunnels and prevent waiting for NPCs to move. Wouldn't be a very fun game otherwise.
  - This also allows for strategic battle locations. Using a defeated npc to block of undefeated ones can allow you to escape if in danger
  - The PC's display symbol `@` takes priority over the NPC's display symbol when on the same tile as an NPC
  - Defeated trainers are given the path's color to indicate that they can be walked over.
- Swimmers can leave water to walk on paths that are near water. (Part of instructor code that I kept because its neat)
  - Swimmers can start battle with PC if PC is next to water. This was done so that Swimmers don't need paths to fight PC. Watch out, though -- they're fast.
- FIXED BUG WITH PACERS
  - Making note so I can come back to this for bug fixes, hopefully
- Buildings, when entered, display a message about how they are not in use right now.
- Trainer list `<t>` displays all trainer types on current map and their relative position to the PC.
  - Trainer list cannot scroll if there are fewer NPCs than display lines (24)
- Changed instructor code so that GATES do not extend out into the environment one tile and are now always flush with the map border. I want the PC to be able to access those tiles like my previous versions.
  - Attempting to enter a gate displays a message that the path is blocked off.
- Added easter eggs
- May have segmentation fault at very high NPC counts

## Version 1.04
This version adds Trainer movement. A number of trainers (or actors) chosen by the user or by default (10) are generated and placed into a trainer map. The map is then used to calculate where the trainers will move. The screen is updated every time the player takes its turn. Hikers and Rivals are the actors added in this version.
- Trees now represented by `4`
- Player does not move in this version, so the screen updates whenever its designated movement time would have been
- Actors are generated by the user using a switch `./game --numtrainers <num>`. Otherwise the number of trainers is 10
  - Invalid input by the user will throw an error and the program will end
  - A value of trainers greater than 75 will also throw an error telling the user it will be a bad experience.
- Generated actors cannot spawn on another actor, a gate, or terrain they cannot traverse
- Generated actors are added to an actorMap at the location of their existance
- Dijkstra's algorithm now returns a distance map, storing the values for long-term use
- After generation, program enters the movement loop, where actors move indefinitely until the program is stopped with `Ctrl-C`
- The movement loop creates a heap, storing the values of actors' move time
- An array of actor_time_t structs is used to store smaller copies of the actors that go into the heap
  - this will hopefully be updated to an array of regular actors in a future version
- The actor with the smallest current time value is removed from the heap, and its movement is calculated
  - Uses distanceMap from dijkstra output, the current time, and the actor map to find where actor will move
  - Actor will move to the smallest cost cell near to it, prioritizing down and right. 
    - This can lead to weird looking pathing, but it is correct. Will hopefully prioritize randomly in a future update
  - After calculating a valid cell the actor will move to, the actorMap is updated to reflect its new location and the actor's cost is updated before it is inserted back into the heap
  - If the selected actor is the player, its time is updated without moving and the map with all recent movements is printed


## Version 1.03
This version adds Player Character placement and NPC movement cost map calculations for the Hiker and Rival type NPCs. Cost
maps are generated for each NPC type and displayed below the current room on output.
- First added the placement of a player character on a random path tile of a room when it is generated
- Player location, room, and NPC type are passed into dijkstra algorithm for calculating a cost map using the heap provided by instructor
- Heap is initilized and the players' location node is passed in to the queue with a value of 0. All other nodes are given a value of INT_MAX
- The lowest cost node of the heap is removed and the neighbors costs are calculated using the removed nodes cost + the cost of the removed nodes type.
   - The removed (center) nodes type cost is calculated using the passed in NPC type and the removed nodes terrain type
- Node cost is updated if the calculated cost is less than the current cost. The updated nodes are then added to the heap.
- If the NPC cannot traverse the terrain of the removed node, that node is skipped
   - Node is skipped if the new cost is negative, meaning the cost overflowed INT_MAX
   - Node is skipped if at the edge of the room so that the borders and gates cannot be traversed
- This process continues until all nodes are removed from the heap
- Finally, the dijkstra algorithm prints the cost maps for the NPC type provided
- This is run for both the Hiker and Rival types every time a room is visited

## Version 1.02
This version creates a world map, saving the grids greated in the previous version so that they can be reaccessed 
and eventual player movement can be implemeted. For now, room by room movent is allowed using various commands
- A world map (401x401) is initilized for storing pointers to rooms. The center room (internally [200, 200]) acts as the starting
  point for our adventure.
- Commands (n, s, e, w, f, q) allow for interaction with the map rooms. n, s, e, and, w provide cardinal movements
  between rooms. `f <x, y>` will transport the user to the room located at (x, y), with (0, 0) being the original room (ex. [-200, 0] would be the top left corner of the map). `q` will quit the game.
   - Errors should be thrown and the current room redisplayed if the input command does not align with any of the preceeding
- Rooms are generated as they are entered and are saved to the world map. If the player is at the edge of the map, the rooms will
  not have gates on the side with the world border. All gates of a room should align with the gates of its neighbors.
- Should the user try to move off the edge of the map, the current room will be displayed again with an error message.
- The current rooms coordinates are displayed below the room itself.
- Marts and Centers have a lower likelihood of appearing the farther from the origin (0, 0) the player travels, based on the 
  Manhattan Distance of the player. The chance deacreases to a flat rate of 5% at distances > 200

## Version 1.01
This version generates a grid "room" (21 x 80) of terrain that will be used for game areas
in future updates.
- Terrain is generated in rectangles of random size which may or may not overlap. This happens infrequenly
enough.
- 3-12 trees are generated for every room, and may or may not be overridden by paths, water, and buildings
- Paths are generated by choosing a random turning point part way through, so they are not all straight to the border, and then left or right.
- Centers and Marts are 2x2 buildings that are generated to connect to, most often times, the nearest path