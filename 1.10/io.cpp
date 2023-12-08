#include <unistd.h>
#include <ncurses.h>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <string>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "pokemon.h"
#include "db_parse.h"

typedef enum __attribute__ ((__packed__)) ball_type {
  pokeball,
  greatball,
  ultraball,
  masterball
} ball_type_t;

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  set_escdelay(0);
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character * const *) v1;
  const character *const *c2 = (const character * const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_bailey:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, BAILEY_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] ==
             DIJKSTRA_PATH_MAX                                            ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(npc **c, uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = dynamic_cast<npc *> (world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  clear();
  mvprintw(0, 0, "Welcome to the Pokemart!");
  
  

  

  int selected = 0;
  int done = 0;
  int exit = 0;

  
  // Wait for user to select correct item
  while(!exit) {
    std::vector<std::pair<std::string, int>> options = {
    {"Potions: " + std::to_string(world.pc.potions) + "\t $200", 200},
    {"Revives: " + std::to_string(world.pc.revives) + "\t $2000", 2000},
    {"Poke Balls: " + std::to_string(world.pc.pokeballs[0]) + "\t $200", 200},
    {"Great Balls: " + std::to_string(world.pc.pokeballs[1]) + "\t $600", 600},
    {"Ultra Balls: " + std::to_string(world.pc.pokeballs[2]) + "\t $800", 800}
    };
    move(1, 0);
    clrtoeol();
    mvprintw(1, 0, "You have $%d", world.pc.pokedollars);
    while(!done) {
      mvprintw(3, 0, "Select item with arrow keys and ENTER key. < to leave\n");
      for(size_t i = 0; i < options.size(); i++) {
        if (i == selected) {
          printw("* ");
        } else {
          printw("  ");
        }
        printw("%s\n", options[i].first.c_str());
      }

      refresh();

      int ch = getch(); // Get user input
      switch (ch) {
        case KEY_UP:
          if (selected > 0) {
          selected--;
          }
          break;
        case KEY_DOWN:
          if (selected < static_cast<int>(options.size()) - 1) {
            selected++;
          }
          break;
        case 10: // ENTER key
          // Only allow selection if user has enough of the item
          if(world.pc.pokedollars >= options[selected].second) {
            done = 1;
          }
          break;
        case '<': // ESC key to go back
          exit = 1;
          done = 1;
          break;
      }
    }
    if(!done || !exit) {
      printw("\nHow many would you like?\n");
      echo();
      curs_set(1);
      int num_items = 0;
      scanw((char *) "%d", &num_items);
      while(num_items * options[selected].second > world.pc.pokedollars) {
        mvprintw(10, 0, "You dont have enough money. Please enter a different value.\n");
        clrtoeol();
        scanw((char *) "%d", &num_items);
      }
      move(10, 0);
      mvprintw(11, 0, "Thank you for your purchase!");
      move(10, 0);
      clrtobot();
      refresh();
      noecho();
      curs_set(0);
      world.pc.pokedollars -= (num_items * options[selected].second);
      switch(selected) {
        case 0:
          world.pc.potions += num_items;
          break;
        case 1:
          world.pc.revives += num_items;
          break;
        case 2:
        case 3:
        case 4:
          world.pc.pokeballs[selected - 2] += num_items;
          break;
      }
      done = 0;
    }
    

    
  }

  clear();
  mvprintw(0, 0, "Come again soon!");
  refresh();
  getch();
  io_display();
}

void io_pokemon_center()
{
  clear();
  mvprintw(0, 0, "Welcome to the Pokemon Center!");

  for(pokemon *p : world.pc.buddy) {
    if(p) {
      p->current_hp = p->get_hp();
    }
  }

  mvprintw(1, 0, "Welcome to the Pokemon Center! Nurse Joy healed all of your Pokemon!");
  mvprintw(2, 0, "Come back soon!");
  mvprintw(4, 0, "Press \'<\' to leave");
  char command = getch();
  while(command != (char)KEY_LEFT && command != '<') {
    command = getch();
  }
}

// clears terminal section for displaying information to user
void io_clear_options() { 
  for (int i = 10; i <= 15; ++i) {
    move(i, 0);
    clrtoeol();
  }
}

// Bag interface for healing and using pokeballs
int io_bag(int isBattle, int isWild) {
  //options for items
  std::vector<std::pair<std::string, int>> options = {
    {"Potions: " + std::to_string(world.pc.potions) + "\t +20 HP", world.pc.potions},
    {"Revives: " + std::to_string(world.pc.revives) + "\t Revive a pkmn to half health", world.pc.revives},
    {"Poke Balls: " + std::to_string(world.pc.pokeballs[0]) + "\t Attempt to capture a wild pkmn", world.pc.pokeballs[0]},
    {"Great Balls: " + std::to_string(world.pc.pokeballs[1]) + "\t A pokeball with a great catch rate", world.pc.pokeballs[1]},
    {"Ultra Balls: " + std::to_string(world.pc.pokeballs[2]) + "\t A pokeball with a phenomenal catch rate", world.pc.pokeballs[2]} 
  };
  int selected = 0;
  int done = 0;
  int validInput = 0;
  int slots = 0;
  int item_used = 0;
  if(!isBattle) { // if not a battle, clear the whole screen, else only clear the bottom for display
    clear();
  }
  else {
    io_clear_options();
  }
  
  // Wait for user to select correct item
  while(!done) {
    mvprintw(10, 0, "Select item with arrow keys and ENTER key. ESC to back out\n");
    for(size_t i = 0; i < options.size(); i++) {
      if (i == selected) {
        printw("* ");
      } else {
        printw("  ");
      }
      printw("%s\n", options[i].first.c_str());
    }

    refresh();

    int ch = getch(); // Get user input
    switch (ch) {
      case KEY_UP:
        if (selected > 0) {
        selected--;
        }
        break;
      case KEY_DOWN:
        if (selected < static_cast<int>(options.size()) - 1) {
          selected++;
        }
        break;
      case 10: // ENTER key
        // Only allow selection if user has enough of the item
        if(selected == 0 && world.pc.potions > 0) {
          done = 1;
        }
        if(selected == 1 && world.pc.revives > 0) {
          done = 1;
        }
        if(selected == 2 && world.pc.pokeballs[pokeball] > 0) {
          done = 1;
        }
        if(selected == 3 && world.pc.pokeballs[greatball] > 0) {
          done = 1;
        }
        if(selected == 4 && world.pc.pokeballs[ultraball] > 0) {
          done = 1;
        }

        break;
      case 27: // ESC key to go back
        if(!isBattle) {
          io_display();
        }
        return 0;
    }
  }
  switch(selected) {
    case 0: // Potion application
      clear();
      mvprintw(0, 0, "Use potion on which pokemon? ESC to cancel\n");
      slots = 0;
      for(pokemon *p : world.pc.buddy) {
        if(p) {
          slots += 1;
          printw("%d: %s (%d / %d)\n", slots, p->get_species(), p->current_hp, p->get_hp());
        }
        refresh();
          

      }
      printw("\n\nSelect value 1 - %d", slots);
      refresh();
      while(!validInput) {
        char choice = getch();
        if(choice == 27) {
          if(!isBattle) {
            io_display();
          }
          return 0;
        }
        selected = choice -'0';
        if(selected >= 1 && selected <= slots && 
          (world.pc.buddy[selected - 1]->current_hp < world.pc.buddy[selected - 1]->get_hp()) &&
          (world.pc.buddy[selected - 1]->current_hp > 0)) {
          validInput = 1;
        }
      }
      world.pc.buddy[selected - 1]->use_potion();
      item_used = 1;
      world.pc.potions--;
      break;
    case 1: // Revive application to pkm
      clear();
      mvprintw(0, 0, "Use revive on which pokemon? ESC to cancel\n");
      slots = 0;
      for(pokemon *p : world.pc.buddy) {
        if(p) {
          slots += 1;
          printw("%d: %s (%d / %d)\n", slots, p->get_species(), p->current_hp, p->get_hp());
        }
        refresh();
      }
      printw("\n\nSelect value 1 - %d", slots);
      refresh();
      while(!validInput) {
        char choice = getch();
        if(choice == 27) {
          if(!isBattle) {
            io_display();
          }
          return 0;
        }
        selected = choice -'0';
        if(selected >= 1 && selected <= slots && 
          (world.pc.buddy[selected - 1]->current_hp == 0)) {
          validInput = 1;
        }
      }
      world.pc.buddy[selected - 1]->use_revive();
      item_used = 1;
      world.pc.revives--;
      break;
    case 2: // Attempt to use pokeball
    case 3:
    case 4: {
      if(isBattle && !isWild && world.pc.pokeballs[selected - 2] > 0) { // waste pokeball if trainer battle
        printw("This enemy trainer blocked your pokeball! What a waste. Press any key to continue"); 
        refresh();
        getch();
        clrtoeol();
        refresh();
      }
      else if(isBattle && isWild && world.pc.pokeballs[selected - 2] > 0) { // Attempt to catch wild pkm 
      }
      else if(!isBattle) { // cannot catch pokemon if not in battle
        printw("There are no pokemon nearby. Press any key to continue");
        refresh();
        getch();
        clrtoeol();
        refresh();
      }
      if(isBattle && world.pc.pokeballs[selected - 2] > 0) { // decrement pokeballs used
        world.pc.pokeballs[selected - 2]--;
        item_used = 2;
      }
      break;
    }


  }
  if(!isBattle) {
    io_display();
  }
  return item_used;

  // mvprintw(1, 1, "Potions: %d", world.pc.potions);
  // mvprintw(2, 1, "Revives: %d", world.pc.revives);
  // mvprintw(3, 1, "Pokeballs: %d", world.pc.pokeballs);
}

void io_print_poke_info(pokemon *p) {

  int * pc_move_index = p->get_move_index();
  int pwr_1 = (moves[pc_move_index[0]].power == INT_MAX) ? 0 : moves[pc_move_index[0]].power;
  int acc_1 = (moves[pc_move_index[0]].accuracy == INT_MAX) ? 0 : moves[pc_move_index[0]].accuracy; 
  int pwr_2 = (moves[pc_move_index[1]].power == INT_MAX) ? 0 : moves[pc_move_index[1]].power;
  int acc_2 = (moves[pc_move_index[1]].accuracy == INT_MAX) ? 0 : moves[pc_move_index[1]].accuracy; 
  printw("%-12s %s",p->get_species(), types[p->type1]);
  if(types[p->type2] != NULL) {
    printw("\t%s", types[p->type2]);
  }
  
  printw("\nHP: %d / %d\n", p->current_hp, p->get_hp());
  printw("Level: %-3d\tGender: %s\n", p->get_level(), p->get_gender_string());
  printw("Move 1: %-27s POWER: %-3d ACCURACY: %-3d\n", p->get_move(0), pwr_1, acc_1);
  printw("Move 2: %-27s POWER: %-3d ACCURACY: %-3d\n", p->get_move(1), pwr_2, acc_2);
  printw("\nATK:    %-3d\tDEF:    %-3d\nSP-ATK: %-3d\tSP-DEF: %-3d\nSPE:    %-3d\n", p->get_atk(), p->get_def(), p->get_spatk(), p->get_spdef(), p->get_speed());


}

void io_print_trainer_info() {
  clear();
  float win_percent = ((float)world.pc.battles_won / (float)(world.pc.battles_lost + world.pc.battles_won)) * 100.0;
  float loss_percent = ((float)world.pc.battles_lost / (float)(world.pc.battles_lost + world.pc.battles_won)) * 100.0;
  if(world.pc.battles_won == 0) {
    win_percent = 0;
  }
  if(world.pc.battles_lost == 0) {
    loss_percent = 0;
  }

  mvprintw(0, 0, "Here are some trainer stats!\n");

  printw("Battles won:  %d (%.2f%%)\n", world.pc.battles_won, win_percent);
  printw("Battles lost: %d (%.2f%%)\n", world.pc.battles_lost, loss_percent);
  printw("Total PokeDollars earned: $%d\n", world.pc.total_pokedollars);
  printw("Pokemon Captured: %d", world.pc.pokemon_captured);
  refresh();
  getch();
  io_display();
}

void io_list_pc_pokemon() {
  clear();
  int num_pkm = 0;
  mvprintw(0, 0, "Inspect your pokemon with left and right arrow keys. ESC to back out.\n");
  int done = 0;
  int viewing_idx = 0;
  for(pokemon *p : world.pc.buddy) {
    if(!p) {
      break;
    }
    num_pkm++;
  }
  while(!done) {
    mvprintw(1, 0, "\nSlot: (%d/%d)\n\n", viewing_idx + 1, num_pkm);
    io_print_poke_info(world.pc.buddy[viewing_idx]);
    refresh();

    int ch = getch(); // Get user input
    switch (ch) {
      case '<':
      case KEY_LEFT:
        if (viewing_idx >= 0) {
          viewing_idx--;
        }
        if(viewing_idx < 0) {
          viewing_idx = num_pkm - 1;
        }
        break;
      case '>': 
      case KEY_RIGHT:
        if (viewing_idx <= num_pkm - 1) {
          viewing_idx++;
        }
        if(viewing_idx == num_pkm) {
          viewing_idx = 0;
        }
        break;
      case 'q':
      case 27: // ESC key to go back
        done = 1;
    }
  }
  io_display();
}

// Check if the given character has any living pokemon remaining
int hasLivingPokemon(character * ch) {
  for(pokemon *p : ch->buddy) {
    if(p) {
      if(p->current_hp > 0) {
        return 1;
      }
    }
  }
  return 0;
}

// Select PC next pokemon from their list
int io_select_pokemon(int wasDeath) {
  io_clear_options();
  if(wasDeath) {
    if(!hasLivingPokemon(&world.pc)) {
      return -1;
    }
    mvprintw(10, 0, "Select your next pokemon\n");
  }
  else {
    mvprintw(10, 0, "Which pokemon would you like to swap to? (ESC to go back)\n");
  }

  std::vector<std::string> pokemon_list; // display trainers list of pokemon to select from
  for(pokemon *p : world.pc.buddy) {
    if(p) {
      pokemon_list.push_back(p->get_species());
    }
  }
  int done = 0;
  int selected = 0;
  while(!done) {
    move(11, 0);
    for(size_t i = 0; i < pokemon_list.size(); i++) {
      if (i == selected) {
        printw("* ");
      } else {
        printw("  ");
      }
      printw("%-15s HP: %3d/%3d\t%-16s %-16s\n", pokemon_list[i].c_str(), world.pc.buddy[i]->current_hp, world.pc.buddy[i]->get_hp(), world.pc.buddy[i]->get_move(0), world.pc.buddy[i]->get_move(1));
    }

    refresh();

    int ch = getch(); // Get user input
    switch (ch) {
      case KEY_UP:
        if (selected > 0) {
        selected--;
        }
        break;
      case KEY_DOWN:
        if (selected < static_cast<int>(pokemon_list.size()) - 1) {
          selected++;
        }
        break;
      case 10: // ENTER key
        // change to selected pkm if its health is > 0 and isnt the current one
        if(world.pc.buddy[selected]->current_hp > 0 && world.pc.active_idx != selected) { 
          world.pc.active_idx = selected;
          return selected;
        }
        break;
      case 27: // ESC
        if(!wasDeath) {
          return -1;
        }
    }
  }
  return -1;
}

// Determine the damage dealt to the defending pokemon based on attacker, defender, and move stats 
int calculate_damage(pokemon *attacker, pokemon *defender, int power, int type_id) {
  int crit_chance = rand() % 256;
  float critical = (crit_chance < (int)(attacker->base_speed / 2)) ? 1.5 : 1.0;
  // mvprintw(17, 0, "%d %d %d", attacker->get_atk(), defender->get_def(), attacker->get_level());
  int random = rand() % 39 + 217;
  float STAB = (type_id == attacker->type1 || type_id == attacker->type2) ? 1.5 : 1.0;
  int damage = ((2 * attacker->get_level()) / 5) + 2;
  int base_damage = ((damage * power * attacker->get_atk() / defender->get_def()) / 50) + 2;
  if(base_damage > 997) {
    base_damage = 997;
  }
  float modified_damage = base_damage * STAB * critical;
  int final_damage = modified_damage * random / 255;
  if(final_damage > defender->current_hp) {
    final_damage = defender->current_hp;
  }
  return (int) final_damage;
}

int calculate_earnings(character *npc, pokemon *p) {
  int level = p->get_level();
  int base;
  switch(npc->symbol) {
    case 'r':
      base = 100;
      break;
    case 'h':
      base = 32;
      break;
    case 'p':
      base = 20;
      break;
    case 'e':
    case 'w':
      base = 15;
      break;
    case 's':
      base = 24;
      break;
    case 'm':
      base = 24;
      break;
  }
  return level * base;
}

// IO for selecting moves under fight option
int io_select_move(int pkm_idx) {
  io_clear_options();
  int done = 0;
  int selected = 0;
  mvprintw(10, 0, "Choose your move UP/DOWN keys and RETURN");
  // print move optsions while user hasnt selected one
  while(!done) {
    int numMoves = 0;
    move(11, 0);
    for(size_t i = 0; i < 2; i++) {
      const char* move = world.pc.buddy[pkm_idx]->get_move(i); 
      if(move && *move != '\0') {
        numMoves++;
        if (i == selected) {
          printw("* ");
        } else {
          printw("  ");
        }
        int * pc_move_index = world.pc.buddy[pkm_idx]->get_move_index();
        int pwr = moves[pc_move_index[i]].power;
        int acc = moves[pc_move_index[i]].accuracy;
        if(pwr == INT_MAX) {
          pwr = 0;
        }
        if(acc == INT_MAX) {
          acc = 0;
        }
        printw("%-16s POW: %3d  ACC: %3d\n", world.pc.buddy[pkm_idx]->get_move(i), pwr, acc);
      }
    }
    refresh();
    int ch = getch(); // Get user input
    switch (ch) {
      case KEY_UP:
        if (selected > 0) {
        selected--;
        }
        break;
      case KEY_DOWN:
        if (selected < numMoves - 1) {
          selected++;
        }
        break;
      case 10: // ENTER key
        return selected;
        break;
      case 27: // ESC to go back
        return -1;
    }
  }
  return -1;
}

// Calculate FIGHT against a trainer
int attack_trainer(pokemon *pc, character *npc_trainer, int *npc_pkm_idx, int attacked, int *pc_pkm_idx) {
  pokemon * npc = npc_trainer->buddy[*npc_pkm_idx];
  int * pc_move_index = pc->get_move_index();
  int * npc_move_index = npc->get_move_index();
  int npc_move = rand() % 2;
  // select first move for npc if null second move
  if(npc_move == 1 && npc->get_move(1)[0] == '\0') { 
    npc_move = 0;
  }
  // obtain move information
  int pc_acc = moves[pc_move_index[attacked]].accuracy;;
  int npc_acc = moves[npc_move_index[npc_move]].accuracy;
  int pc_prio = moves[pc_move_index[attacked]].priority;
  int npc_prio = moves[npc_move_index[npc_move]].priority;
  int pc_power = moves[pc_move_index[attacked]].power;
  int npc_power = moves[npc_move_index[npc_move]].power;
  int pc_type_id = moves[pc_move_index[attacked]].type_id;
  int npc_type_id = moves[npc_move_index[npc_move]].type_id; 
  // set values to 0 if INT_MAX
  if(pc_acc == INT_MAX) {
    pc_acc = 0;
  }
  if(npc_acc == INT_MAX) {
    npc_acc = 0;
  }
  if(pc_power == INT_MAX) {
    pc_power = 0;
  }
  if(npc_power == INT_MAX) {
    npc_power = 0;
  }
  int pc_dmg_taken;
  int npc_dmg_taken;
  // store move order based if player is attacking
  std::vector<pokemon *> turn_order;
  if(attacked == -1) {
    pc_dmg_taken = calculate_damage(npc, pc, npc_power, npc_type_id); 
    mvprintw(18, 0, "The opposing %s used %s\n", npc->get_species(), moves[npc_move_index[npc_move]].identifier);
    if(rand() % 100 < npc_acc) {
      pc->current_hp -= pc_dmg_taken;
      printw("%s took %d damage!\n", pc->get_species(), pc_dmg_taken);
    }
    else {
      printw("The attack misses!\n");
    }
    refresh();
    getch();
    if(pc->current_hp <= 0) {
      if(hasLivingPokemon(&world.pc)) {
        printw("%s fainted! Press any key to choose your next pokemon", pc->get_species());
      } else {
        printw("All of your pokemon have fainted.\nYou lose!");
        world.pc.battles_lost++;
      }
      refresh();
      getch();
      pc->current_hp = 0;
      int next_index = io_select_pokemon(1);
      world.pc.active_idx = next_index;
      *pc_pkm_idx = next_index;
      world.pc.active_idx = next_index;
      return 0;
    }
    return 0;
  }
  // calculate move priorityes and pass into turn order
  else if(pc_prio > npc_prio) {
    turn_order.push_back(pc);
    turn_order.push_back(npc);
  }
  else if(npc_prio > pc_prio) {
    turn_order.push_back(npc);
    turn_order.push_back(pc);
  }
  else if(pc_prio == npc_prio) {
    if(pc->get_speed() > npc->get_speed()) {
      turn_order.push_back(pc);
      turn_order.push_back(npc);
    }
    else if (npc->get_speed() > pc->get_speed()) {
      turn_order.push_back(npc);
      turn_order.push_back(pc);
    }
    else {
      if(rand() % 2 == 0) {
        turn_order.push_back(pc);
        turn_order.push_back(npc);
      }
      else {
        turn_order.push_back(npc);
        turn_order.push_back(pc);
      }
    }
  }
  // Attack based on move order
  for(pokemon * p : turn_order) {
    if(p == pc) {
      npc_dmg_taken = calculate_damage(pc, npc, pc_power, pc_type_id);
      mvprintw(18, 0, "Your %s used %s\n", pc->get_species(), moves[pc_move_index[attacked]].identifier);
      if(rand() % 100 < pc_acc) {
        npc->current_hp -= npc_dmg_taken;
        printw("%s took %d damage!\n", npc->get_species(), npc_dmg_taken);
      }
      else {
        printw("The attack misses!\n");
      }
      refresh(); 
      getch();
      if(npc->current_hp <= 0) { // if the pokemon faints, swap out or end battle
        printw("The opposing %s fainted.\n", npc->get_species());
        if(hasLivingPokemon(npc_trainer)) {
          npc_pkm_idx++;
          printw("Your foe sends out %s!", npc_trainer->buddy[*npc_pkm_idx]->get_species()); 
        }
        else {
          int  earnings = calculate_earnings(npc_trainer, npc_trainer->buddy[*npc_pkm_idx]);
          world.pc.pokedollars += earnings;
          world.pc.total_pokedollars += earnings;
          world.pc.battles_won++;
          printw("The opposing trainer has no more pokemon. You win!\n");
          printw("You receive %d pokedollars!", earnings);
          
        }
        refresh();
        getch();
        return 1;
      }
    }
    else if(p == npc) {
      pc_dmg_taken = calculate_damage(npc, pc, npc_power, npc_type_id);
      mvprintw(18, 0, "The opposing %s used %s\n", npc->get_species(), moves[npc_move_index[npc_move]].identifier);
      if(rand() % 100 < npc_acc) {
        pc->current_hp -= pc_dmg_taken;
        printw("%s took %d damage!\n", pc->get_species(), pc_dmg_taken);
      }
      else {
        printw("The attack misses!\n");
      }
      refresh();
      getch();
      if(pc->current_hp <= 0) { // if PC pokemon faints, swap out or end battle
        if(hasLivingPokemon(&world.pc)) {
          printw("%s fainted! Press any key to choose your next pokemon", pc->get_species());
        } else {
          printw("All of your pokemon have fainted.\nYou lose!");
          world.pc.battles_lost++;
        }
        refresh();
        getch();
        pc->current_hp = 0;
        int next_index = io_select_pokemon(1);
        world.pc.active_idx = next_index;
        *pc_pkm_idx = next_index;
        world.pc.active_idx = next_index;
        return 0;
      }
    }
  }
  return 0;
}

// FIGHT command for wild pkm encounter
int attack_wild(pokemon *pc, pokemon *npc, int attacked, int *pc_pkm_idx) {  
  int * pc_move_index = pc->get_move_index();
  int * npc_move_index = npc->get_move_index();
  int npc_move = rand() % 2;
  if(npc_move == 1 && npc->get_move(1)[0] == '\0') { // null move selection swapped to move 1
    npc_move = 0;
  }
  int pc_acc = moves[pc_move_index[attacked]].accuracy;;
  int npc_acc = moves[npc_move_index[npc_move]].accuracy;
  int pc_prio = moves[pc_move_index[attacked]].priority;
  int npc_prio = moves[npc_move_index[npc_move]].priority;
  int pc_power = moves[pc_move_index[attacked]].power;
  int npc_power = moves[npc_move_index[npc_move]].power;
  int pc_type_id = moves[pc_move_index[attacked]].type_id;
  int npc_type_id = moves[npc_move_index[npc_move]].type_id; 
  if(pc_acc == INT_MAX) {
    pc_acc = 0;
  }
  if(npc_acc == INT_MAX) {
    npc_acc = 0;
  }
  if(pc_power == INT_MAX) {
    pc_power = 0;
  }
  if(npc_power == INT_MAX) {
    npc_power = 0;
  }
  int pc_dmg_taken;
  int npc_dmg_taken;
  // store turn order
  std::vector<pokemon *> turn_order;
  if(attacked == -1) {
    pc_dmg_taken = calculate_damage(npc, pc, npc_power, npc_type_id);
    mvprintw(18, 0, "The wild %s used %s\n", npc->get_species(), moves[npc_move_index[npc_move]].identifier);
    if(rand() % 100 < npc_acc) {
      pc->current_hp -= pc_dmg_taken;
      printw("%s took %d damage!\n", pc->get_species(), pc_dmg_taken);
    }
    else {
      printw("The attack misses!\n");
    }
    refresh();
    getch();
    if(pc->current_hp <= 0) {
      if(hasLivingPokemon(&world.pc)) {
        printw("%s fainted! Press any key to choose your next pokemon", pc->get_species());
      } else {
        printw("All of your pokemon have fainted.\nYou lose!");
      }
      refresh();
      getch();
      pc->current_hp = 0;
      int next_index = io_select_pokemon(1);
      world.pc.active_idx = next_index;
      *pc_pkm_idx = next_index;
      world.pc.active_idx = next_index;
      return 0;
    }
    return 0;
  }
  // move priority initilization
  else if(pc_prio > npc_prio) {
    turn_order.push_back(pc);
    turn_order.push_back(npc);
  }
  else if(npc_prio > pc_prio) {
    turn_order.push_back(npc);
    turn_order.push_back(pc);
  }
  else if(pc_prio == npc_prio) {
    if(pc->get_speed() > npc->get_speed()) {
      turn_order.push_back(pc);
      turn_order.push_back(npc);
    }
    else if (npc->get_speed() > pc->get_speed()) {
      turn_order.push_back(npc);
      turn_order.push_back(pc);
    }
    else {
      if(rand() % 2 == 0) {
        turn_order.push_back(pc);
        turn_order.push_back(npc);
      }
      else {
        turn_order.push_back(npc);
        turn_order.push_back(pc);
      }
    }
  }
  // attack based on move order
  for(pokemon * p : turn_order) {
    if(p == pc) { // pc attacks
      npc_dmg_taken = calculate_damage(pc, npc, pc_power, pc_type_id);
      mvprintw(18, 0, "Your %s used %s\n", pc->get_species(), moves[pc_move_index[attacked]].identifier);
      if(rand() % 100 < pc_acc) {
        npc->current_hp -= npc_dmg_taken;
        printw("%s took %d damage!\n", npc->get_species(), npc_dmg_taken);
      }
      else {
        printw("The attack misses!\n");
      }
      refresh(); 
      getch();
      if(npc->current_hp <= 0) { // if wild pokemon faints, you win
        return 1;
      }
    }
    else if(p == npc) { // npc attacks
      pc_dmg_taken = calculate_damage(npc, pc, npc_power, npc_type_id);
      mvprintw(18, 0, "The wild %s used %s\n", npc->get_species(), moves[npc_move_index[npc_move]].identifier);
      if(rand() % 100 < npc_acc) {
        pc->current_hp -= pc_dmg_taken;
        printw("%s took %d damage!\n", pc->get_species(), pc_dmg_taken);
      }
      else {
        printw("The attack misses!\n");
      }
      refresh();
      getch();
      if(pc->current_hp <= 0) {
        if(hasLivingPokemon(&world.pc)) { 
          printw("%s fainted! Press any key to choose your next pokemon", pc->get_species());
        } else { // if all pc pokemon faints, battle ends
          printw("All of your pokemon have fainted.\nYou lose!");
        }
        refresh();
        getch();
        pc->current_hp = 0;
        int next_index = io_select_pokemon(1);
        world.pc.active_idx = next_index;
        *pc_pkm_idx = next_index;
        world.pc.active_idx = next_index;
        return 0;
      }
    }
  }
  return 0;
}

// IO interface for trainer battles
void io_battle(character *aggressor, character *defender) {
  npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);
  int battling = 1;
  int npc_pkm_idx = 0;
  int pc_pkm_idx = 0;
  for(pokemon *p : world.pc.buddy) {
    if(p) {
      if(p->current_hp == 0) {
        pc_pkm_idx++;
      }
      else {
        break;
      }
    }
  }
  world.pc.active_idx = pc_pkm_idx;
  int next_idx = pc_pkm_idx;
  int selected = 0;
  int item_used = 0;
  int win = 0;
  int num_pkm = 0;
  for(pokemon *i : n->buddy) {
    if(i) {
      num_pkm++;
    }
  }

  
  // options list
  std::vector<std::string> options = {"FIGHT", "POKEMON", "BAG", "RUN"};

  clear();
  mvprintw(0, 0, "Trainer %c at location x:%d y:%d has challenged you to a battle!\n", n->symbol, n->pos[dim_x], n->pos[dim_y]);
  printw("Trainer %c sends out %s!\n", n->symbol, n->buddy[npc_pkm_idx]->get_species());
  printw("Go, %s!\n", world.pc.buddy[pc_pkm_idx]->get_species());

  printw("Press any key to begin the battle!");
  refresh();
  getch();
  clear();
  
  while(battling) {
    int confirmed = 0;
    int attacked = -1;
    // display pkm info
    mvprintw(0, 0, "Opponent's %s (%d / %d)\n HP: %d / %d\n", n->buddy[npc_pkm_idx]->get_species(), npc_pkm_idx + 1, num_pkm, n->buddy[npc_pkm_idx]->current_hp, n->buddy[npc_pkm_idx]->get_hp());
    printw(" lvl: %-3d\t%s\t%s", n->buddy[npc_pkm_idx]->get_level(), n->buddy[npc_pkm_idx]->get_move(0), n->buddy[npc_pkm_idx]->get_move(1));
    mvprintw(5, 0, "My %s\n HP: %d / %d\n", world.pc.buddy[pc_pkm_idx]->get_species(), world.pc.buddy[pc_pkm_idx]->current_hp, world.pc.buddy[pc_pkm_idx]->get_hp());
    printw(" lvl: %-3d", world.pc.buddy[pc_pkm_idx]->get_level());
    
    while(!confirmed) {
      clear();
      mvprintw(0, 0, "Opponent's %s (%d / %d)\n HP: %d / %d\n", n->buddy[npc_pkm_idx]->get_species(), npc_pkm_idx + 1, num_pkm, n->buddy[npc_pkm_idx]->current_hp, n->buddy[npc_pkm_idx]->get_hp());
      printw(" lvl: %-3d\t%s\t%s", n->buddy[npc_pkm_idx]->get_level(), n->buddy[npc_pkm_idx]->get_move(0), n->buddy[npc_pkm_idx]->get_move(1));
      mvprintw(5, 0, "My %s\n HP: %d / %d\n", world.pc.buddy[pc_pkm_idx]->get_species(), world.pc.buddy[pc_pkm_idx]->current_hp, world.pc.buddy[pc_pkm_idx]->get_hp());
      printw(" lvl: %-3d", world.pc.buddy[pc_pkm_idx]->get_level());
      mvprintw(9, 0, "Choose with UP/DOWN and RETURN\n");
      int choice = -1;
      for(size_t i = 0; i < options.size(); i++) {
        if (i == selected) {
          printw("* ");
        } else {
          printw("  ");
        }
        printw("%s\n", options[i].c_str());
      }

      refresh();

      int ch = getch();
      switch (ch) {
        case KEY_UP:
          if (selected > 0) {
          selected--;
          }
          break;
        case KEY_DOWN:
          if (selected < static_cast<int>(options.size()) - 1) {
            selected++;
          }
          break;
        case 10: // ENTER key
          move(9, 0);
          clrtoeol();
          choice = selected;
          break;
      }
      if(choice >= 0) {
        choice = -1;
        switch(selected) {
          case 0: // FIGHT
            attacked = io_select_move(pc_pkm_idx);
            if(attacked != -1) {
              confirmed = 1;
            }
            break;
          case 1: // POKEMON
            next_idx = io_select_pokemon(0);
            if(next_idx != -1) {
              pc_pkm_idx = next_idx;
              confirmed = 1;
              world.pc.active_idx = pc_pkm_idx;
            }
            break;
          case 2:
            item_used = io_bag(1, 0);
            if(item_used > 0) {
              confirmed = 1;
            }
            break;
          case 3:
            io_clear_options();
            mvprintw(10, 0, "You cannot flee Trainer battles! Press any key to continue");
            refresh();
            getch();
            clrtoeol();
            break;
        }
      }
    }
    int kill;
    switch(selected) {
      case 0: // FIGHT option
        kill = attack_trainer(world.pc.buddy[pc_pkm_idx], n, &npc_pkm_idx, attacked, &pc_pkm_idx);
        
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          for(pokemon * p : n->buddy) {
            if(p) {
              p->current_hp = p->get_hp();
            }
          }
          break;
        }
        if(hasLivingPokemon(n) && kill) {
          npc_pkm_idx++;
          break;
        }
        else if(!hasLivingPokemon(n) && kill) {
          battling = 0;
          win = 1;
          break;
        }
        break;
      case 1: // POKEMON option
        mvprintw(17, 0, "You spend your turn swapping pokemon");
        kill = attack_trainer(world.pc.buddy[pc_pkm_idx], n, &npc_pkm_idx, attacked, &pc_pkm_idx);
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          for(pokemon * p : n->buddy) {
            if(p) {
              p->current_hp = p->get_hp();
            }
          }
          break;
        }
        refresh();
        break;
      case 2: // BAG option
        mvprintw(17, 0, "You spend your turn using an item");
        kill = attack_trainer(world.pc.buddy[pc_pkm_idx], n, &npc_pkm_idx, attacked, &pc_pkm_idx);
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          for(pokemon * p : n->buddy) {
            if(p) {
              p->current_hp = p->get_hp();
            }
          }
          break;
        }
        refresh();
        break;
      case 3: // RUN (not work in trainer battle)
        break;
    }
  }

  io_display();
  refresh();
  if(win) { // set npc move type on win
    n->defeated = 1;
    if (n->ctype == char_hiker || n->ctype == char_rival) {
      n->mtype = move_wander;
    } 
  }
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<npc *> (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if ((dynamic_cast<npc *>
                (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]))) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      DIJKSTRA_PATH_MAX) {
    return 1;
  }

  if (world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_gate &&
      dest[dim_y] != world.pc.pos[dim_y]                       &&
      dest[dim_x] != world.pc.pos[dim_x]) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, (char *) "%d", &x);
  } while (x < -200 || x > 200);
  do {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, (char *) "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      turn_not_consumed = 1;
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;    
    case 'B':
      io_bag(0, 0);
      turn_not_consumed = 1;
      break;
    case 'P':
      io_list_pc_pokemon();
      turn_not_consumed = 1;
      break;
    case 'i':
    case 'I':
      io_print_trainer_info();
      turn_not_consumed = 1;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}

// throw a pokeball at a wild pokemon. For now, always catches
int throw_pokeball(character *ch, pokemon *p, int ball_type) {
  int i;
  int N = 0;
  int ball_value = 12;
  if(ball_type == greatball) {
    ball_value = 8;
  }
  switch(ball_type) {
    case pokeball:
      N = rand() % 256;
      break;
    case greatball:
      N = rand() % 201;
      break;
    case ultraball:
      N = rand() % 151;
      break;
    case masterball:
      return 1;
      break;
  }
  int capture_rate = p->capture_rate;

  if(N > capture_rate) {
    return 0;
  }

  int M = rand() % 256;
  int f = (p->get_hp() * 255 * 4) / (p->current_hp * ball_value);
  if(f > 255) {
    f = 255;
  }
  if(f < 1) {
    f = 1;
  }
  for(i = 0; i < 6; i++) {
    if(!ch->buddy[i] && f >= M) {
      ch->buddy[i] = p;
      world.pc.pokemon_captured += 1;
      return 1;
    }
  }
  return 0;
}

// IO for encountering a wild pokemon
void io_encounter_pokemon()
{
  pokemon *p;
  p = new pokemon();
  int battling = 1;
  // battle options
  std::vector<std::string> options = {"FIGHT", "POKEMON", "BAG", "RUN"};
  int pc_pkm_idx = 0;
  for(pokemon *p : world.pc.buddy) {
    if(p) {
      if(p->current_hp == 0) {
        pc_pkm_idx++;
      }
      else {
        break;
      }
    }
  }
  world.pc.active_idx = pc_pkm_idx;
  int next_idx = pc_pkm_idx;
  int item_used = 0;
  int selected = 0;
  int captured = 0;


  clear();
  move(0,0);
  clrtoeol();
  printw("You encountered a wild %s!\n", p->get_species());
  printw("Go, %s!\n", world.pc.buddy[pc_pkm_idx]->get_species());

  printw("Press any key to begin the battle!");
  refresh();
  getch();
  

  while(battling) {
    int confirmed = 0;
    int attacked = -1;
    // display pokmeon info for battle
    mvprintw(0, 0, "Wild %s\n HP: %d / %d\n", p->get_species(), p->current_hp, p->get_hp());
    printw(" lvl: %-3d\t%s\t%s", p->get_level(), p->get_move(0), p->get_move(1)); 
    mvprintw(5, 0, "My %s\n HP: %d / %d\n", world.pc.buddy[pc_pkm_idx]->get_species(), world.pc.buddy[pc_pkm_idx]->current_hp, world.pc.buddy[pc_pkm_idx]->get_hp());
    printw(" lvl: %-3d", world.pc.buddy[pc_pkm_idx]->get_level());
    
    while(!confirmed) {
      clear();
      mvprintw(0, 0, "Wild %s\n HP: %d / %d\n", p->get_species(), p->current_hp, p->get_hp());
      printw(" lvl: %-3d\t%s\t%s", p->get_level(), p->get_move(0), p->get_move(1)); 
      mvprintw(5, 0, "My %s\n HP: %d / %d\n", world.pc.buddy[pc_pkm_idx]->get_species(), world.pc.buddy[pc_pkm_idx]->current_hp, world.pc.buddy[pc_pkm_idx]->get_hp());
      printw(" lvl: %-3d", world.pc.buddy[pc_pkm_idx]->get_level());
      mvprintw(9, 0, "Choose with UP/DOWN and RETURN\n");
      int choice = -1;
      for(size_t i = 0; i < options.size(); i++) {
        if (i == selected) {
          printw("* ");
        } else {
          printw("  ");
        }
        printw("%s\n", options[i].c_str());
      }

      refresh();

      int ch = getch();
      switch (ch) {
        case KEY_UP:
          if (selected > 0) {
          selected--;
          }
          break;
        case KEY_DOWN:
          if (selected < static_cast<int>(options.size()) - 1) {
            selected++;
          }
          break;
        case 10: // ENTER key
          move(9, 0);
          clrtoeol();
          choice = selected;
          break;
      }
      if(choice >= 0) {
        choice = -1;
        switch(selected) {
          case 0: // FIGHT
            attacked = io_select_move(pc_pkm_idx);
            if(attacked != -1) {
              confirmed = 1;
            }
            break;
          case 1: // POKEMON
            next_idx = io_select_pokemon(0);
            if(next_idx != -1) {
              pc_pkm_idx = next_idx;
              confirmed = 1;
              world.pc.active_idx = pc_pkm_idx;
            }
            break;
          case 2: // BAG
            item_used = io_bag(1, 1);
            if(item_used > 0) {
              confirmed = 1;
            }
            if(item_used == 2 || item_used == 3 || item_used == 4) {
              captured = throw_pokeball(&world.pc, p, item_used - 2);
              io_clear_options();
              mvprintw(10, 0, "You throw a pokeball!\n");
              refresh();
              getch();
              if(captured == 1) {
                battling = 0;
                printw("\nYou captured the %s", p->get_species());
              }
              else {
                printw("\nThe wild %s broke free!", p->get_species());
              }
              refresh();
              getch();
            }
            break;
          case 3: // RUN
            io_clear_options();
            mvprintw(10, 0, "You attempt to flee! (Press any key)\n");
            if(rand() % 10 <= 3) {
              printw("Could not get away!\n");
            }
            else {
              printw("Got away safely!\n");
              battling = 0;
            }
            refresh();
            getch();
            
            confirmed = 1;
            break;
        }
      }
    }
    int kill;
    if(rand() % 100 < 9 && battling) { // 10% change for pkm to flee battle
      mvprintw(17, 0, "The wild %s fled the battle! Press any key to continue", p->get_species()); 
      battling = 0;
      refresh();
      getch();
    }
    if(battling){
    switch(selected) {
      case 0: // FIGHT
        kill = attack_wild(world.pc.buddy[pc_pkm_idx], p, attacked, &pc_pkm_idx); // change final value here IMPORTANT
        
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          break;
        }
        else if(p->current_hp <= 0 && kill) {
          battling = 0;
          break;
        }
        break;

      case 1: // POKEMON
        mvprintw(17, 0, "You spend your turn swapping pokemon");
        kill = attack_wild(world.pc.buddy[pc_pkm_idx], p, attacked, &pc_pkm_idx);
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          break;
        }
        refresh();
        break;
      case 2: // BAG
        mvprintw(17, 0, "You spend your turn using an item");
        kill = attack_wild(world.pc.buddy[pc_pkm_idx], p, attacked, &pc_pkm_idx);
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          break;
        }
        refresh();
        break;
      case 3: // RUN
        mvprintw(17, 0, "You spend your turn trying to flee");
        kill = attack_wild(world.pc.buddy[pc_pkm_idx], p, attacked, &pc_pkm_idx);
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          io_pokemon_center();
          io_display();
          refresh();
          break;
        }
        refresh();
        break;

    }
    }

  }



  // mvprintw(1, 0, "move 1: %s", p.move1.c_str());
  // mvprintw(2, 0, "move 2: %s", p.move2.c_str());
  // mvprintw(4, 0, "HP: %d ATK: %d DEF: %d SP-ATK: %d SP-DEF: %d SPE: %d", p.hp, p.atk, p.def, p.spa, p.spd, p.spe);
  // mvprintw(5, 0, "Press any key to leave");
  refresh();

  // Later on, don't delete if captured
  if(!captured) {
    delete p;
  }
}

void io_choose_starter()
{
  class pokemon *choice[3];
  int i;
  bool again = true;
  
  choice[0] = new class pokemon();
  choice[1] = new class pokemon();
  choice[2] = new class pokemon();

  echo();
  curs_set(1);
  do {
    mvprintw( 4, 20, "Before you are three Pokemon, each of");
    mvprintw( 5, 20, "which wants absolutely nothing more");
    mvprintw( 6, 20, "than to be your best buddy forever.");
    mvprintw( 8, 20, "Unfortunately for them, you may only");
    mvprintw( 9, 20, "pick one.  Choose wisely.");
    mvprintw(11, 20, "   1) %s", choice[0]->get_species());
    mvprintw(12, 20, "   2) %s", choice[1]->get_species());
    mvprintw(13, 20, "   3) %s", choice[2]->get_species());
    mvprintw(15, 20, "Enter 1, 2, or 3: ");

    refresh();
    i = getch();

    if (i == '1' || i == '2' || i == '3') {
      world.pc.buddy[0] = choice[(i - '0') - 1];
      delete choice[(i - '0') % 3];
      delete choice[((i - '0') + 1) % 3];
      again = false;
    }
  } while (again);
  noecho();
  curs_set(0);
}
