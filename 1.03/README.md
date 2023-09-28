# 327 Pokemon Game README - Gannon Guess

## Version 1.03

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