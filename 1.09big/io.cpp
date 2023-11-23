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
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0                 ||
           world.cur_map->map[dest[dim_y]][dest[dim_x]] != ter_center);

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

  world.pc.revives = MAX_REVIVES;
  world.pc.potions = MAX_POTIONS;
  world.pc.pokeballs = MAX_POKEBALLS;

  mvprintw(1, 0, "You've replinished all of your items");
  mvprintw(2, 0, "Come again soon!");
  mvprintw(4, 0, "Press \'<\' to leave");
  char command = getch();
  while(command != (char)KEY_LEFT && command != '<') {
    command = getch();
  }
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

  mvprintw(1, 0, "Nurse Joy healed all of your pokemon");
  mvprintw(2, 0, "Come back soon!");
  mvprintw(4, 0, "Press \'<\' to leave");
  char command = getch();
  while(command != (char)KEY_LEFT && command != '<') {
    command = getch();
  }
}

void io_clear_options() {
  for (int i = 10; i <= 15; ++i) {
    move(i, 0);
    clrtoeol();
  }
}

int io_bag(int isBattle, int isWild) {
  
  std::vector<std::pair<std::string, int>> options = {
    {"Potions: " + std::to_string(world.pc.potions), world.pc.potions},
    {"Revives: " + std::to_string(world.pc.revives), world.pc.revives},
    {"Pokeballs: " + std::to_string(world.pc.pokeballs), world.pc.pokeballs}
  };
  int selected = 0;
  int done = 0;
  int validInput = 0;
  int slots = 0;
  int item_used = 0;
  if(!isBattle) {
    clear();
  }
  else {
    io_clear_options();
  }
  
  
  while(!done) {
    mvprintw(10, 0, "You check your bag: (Select item with arrow keys and ENTER key)\n");
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
        // Do something with the selected option (e.g., print it)
        done = 1;
        break;
      case 27: // ESC key
        if(!isBattle) {
          io_display();
        }
        return 0;
    }
  }
  switch(selected) {
    case 0: // Potion
      clear();
      mvprintw(0, 0, "Use potion on which pokemon?\n");
      slots = 0;
      for(pokemon *p : world.pc.buddy) {
        if(p) {
          slots += 1;
          printw("%d: %s (%d / %d)\n", slots, p->get_species(), p->current_hp, p->get_hp());
        }
        refresh();
          

      }
      printw("Select value 1 - %d: ", slots);
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
    case 1:
      clear();
      mvprintw(0, 0, "Use revive on which pokemon?\n");
      slots = 0;
      for(pokemon *p : world.pc.buddy) {
        if(p) {
          slots += 1;
          printw("%d: %s (%d / %d)\n", slots, p->get_species(), p->current_hp, p->get_hp());
        }
        refresh();
      }
      printw("Select value 1 - %d: ", slots);
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
    case 2:
      if(isBattle && world.pc.pokeballs > 0) {
        world.pc.pokeballs--;
        item_used = 2;
      }
      if(isBattle && !isWild && world.pc.pokeballs > 0) {
        printw("This enemy trainer blocked your pokeball! What a waste. Press any key to continue");
        refresh();
        getch();
        clrtoeol();
        refresh();
      }
      else if(!isBattle) {
        printw("There are no pokemon nearby. Press any key to continue");
        refresh();
        getch();
        clrtoeol();
        refresh();
      }
      break;



  }
  if(!isBattle) {
    io_display();
  }
  return item_used;

  // mvprintw(1, 1, "Potions: %d", world.pc.potions);
  // mvprintw(2, 1, "Revives: %d", world.pc.revives);
  // mvprintw(3, 1, "Pokeballs: %d", world.pc.pokeballs);
}

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

int io_select_pokemon(int wasDeath) {
  io_clear_options();
  if(wasDeath) {
    if(!hasLivingPokemon(&world.pc)) {
      return -1;
    }
    mvprintw(10, 0, "Select your next pokemon (ESC to go back)\n");
  }
  else {
    mvprintw(10, 0, "Which pokemon would you like to swap to? (ESC to go back)\n");
  }

  std::vector<std::string> pokemon_list;
  for(pokemon *p : world.pc.buddy) {
    if(p) {
      pokemon_list.push_back(p->get_species());
    }
  }
  int done = 0;
  int selected = 0;
  int choice;
  while(!done) {
    move(11, 0);
    for(size_t i = 0; i < pokemon_list.size(); i++) {
      if (i == selected) {
        printw("* ");
      } else {
        printw("  ");
      }
      printw("%s \t\tHP: %d / %d\n", pokemon_list[i].c_str(), world.pc.buddy[i]->current_hp, world.pc.buddy[i]->get_hp());
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
        if(world.pc.buddy[selected]->current_hp > 0 && world.pc.active_idx != selected) {
          world.pc.active_idx = selected;
          return selected;
        }
        break;
      case 27:
        return -1;
    }
  }
  return -1;
}

int io_select_move(int pkm_idx) {
  io_clear_options();
  int done = 0;
  int selected = 0;
  mvprintw(10, 0, "Choose your move");
  
  while(!done) {
    int numMoves = 0;
    move(11, 0);
    for(size_t i = 0; i < 2; i++) {
      if(world.pc.buddy[pkm_idx]->get_move(i) != nullptr) {
        numMoves++;
        if (i == selected) {
          printw("* ");
        } else {
          printw("  ");
        }
        printw("%s\n", world.pc.buddy[pkm_idx]->get_move(i));
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
      case 27:
        return -1;
    }
  }
  return -1;
}

int calculate_damage(pokemon *attacker, int power, int type_id) {
  int crit_chance = rand() % 256;
  float critical = (crit_chance < (int)(attacker->base_speed / 2)) ? 1.5 : 1.0;
  int type = 1;
  // int random = rand() % 16 + 85;
  float STAB = (type_id == attacker->type1 || type_id == attacker->type2) ? 1.5 : 1.0;
  float damage = ((2.0 * attacker->get_level()) / 5.0) + 2.0;
  damage = ((damage * power * (attacker->get_atk() / attacker->get_def())) / 50) + 2.0;
  damage = damage * critical * STAB * type /* random*/;
  return damage;
}

void attack(pokemon *pc, pokemon *npc, int attacked) {
  int * pc_move_index = pc->get_move_index();
  int * npc_move_index = npc->get_move_index();
  int npc_move = rand() % 2;
  if(!npc->get_move(1)) {
    npc_move = 0;
  }
  int pc_acc = moves[pc_move_index[attacked]].accuracy;;
  int npc_acc = moves[npc_move_index[npc_move]].accuracy;
  int pc_prio = moves[pc_move_index[attacked]].priority;
  int npc_prio = moves[npc_move_index[npc_move]].priority;
  int pc_power = moves[pc_move_index[attacked]].power;
  int npc_power = moves[npc_move_index[npc_move]].power;
  int npc_attacked = 0;
  int pc_attacked = 0;
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

  if(attacked == -1) {
    int damag = calculate_damage(npc, npc_power, npc_type_id);
    pc->current_hp -= damag;
    if(pc->current_hp <= 0) {
      pc->current_hp = 0;
      io_select_pokemon(1);
      return;
    }
    mvprintw(20, 0, "damage dealt: %d %d", damag, pc->current_hp);
    refresh();
    getch();
    return;
  }
  else if(pc_prio > npc_prio) {
    
  }
}

void io_battle(character *aggressor, character *defender) {
  int i;
  npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);
  int battling = 1;
  int npc_pkm_idx = 0;
  int pc_pkm_idx = 0;
  int next_idx = pc_pkm_idx;
  int selected = 0;
  int choice = 0;
  int item_used = 0;
  int win = 0;

  

  std::vector<std::string> options = {"Fight", "Pokemon", "Bag", "Run"};

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
    
    mvprintw(0, 0, "Opponent's %s\n HP: %d / %d", n->buddy[npc_pkm_idx]->get_species(), n->buddy[npc_pkm_idx]->current_hp, n->buddy[npc_pkm_idx]->get_hp());
    mvprintw(5, 0, "My %s\n HP: %d / %d", world.pc.buddy[pc_pkm_idx]->get_species(), world.pc.buddy[pc_pkm_idx]->current_hp, world.pc.buddy[pc_pkm_idx]->get_hp());
    
    while(!confirmed) {
      clear();
      mvprintw(0, 0, "Opponent's %s\n HP: %d / %d", n->buddy[npc_pkm_idx]->get_species(), n->buddy[npc_pkm_idx]->current_hp, n->buddy[npc_pkm_idx]->get_hp());
      mvprintw(5, 0, "My %s\n HP: %d / %d", world.pc.buddy[pc_pkm_idx]->get_species(), world.pc.buddy[pc_pkm_idx]->current_hp, world.pc.buddy[pc_pkm_idx]->get_hp());
      move(10, 0);
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
          choice = selected;
          break;
      }
      if(choice >= 0) {
        choice = -1;
        switch(selected) {
          case 0:
            attacked = io_select_move(pc_pkm_idx);
            if(attacked != -1) {
              confirmed = 1;
            }
            break;
          case 1:
            next_idx = io_select_pokemon(0);
            if(next_idx != -1) {
              pc_pkm_idx = next_idx;
              confirmed = 1;
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
    switch(selected) {
      case 0:
        attack(world.pc.buddy[pc_pkm_idx], n->buddy[0],  -1); // change final value here IMPORTANT
        
        if(!hasLivingPokemon(&world.pc)) {
          battling = 0;
          mvprintw(15, 0, "All of your pokemon have fainted. You black out and wake up in the pokemon center");
          world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = nullptr;
          io_teleport_pc(world.pc.pos);
          io_pokemon_center();
          break;
        }
        
        //mvprintw(15, 0, "NPC dealt %d damage", damage);
        break;
      case 1:
        mvprintw(15, 0, "You spend your turn swapping pokemon");
        refresh();
        getch();
        break;
      case 2:
        mvprintw(15, 0, "You spend your turn using an item");
        refresh();
        getch();
        break;
      case 3:
        mvprintw(15, 0, "You spend your turn attempting to flee");
        refresh();
        getch();
        break;
    }
  }
  io_display();
  refresh();
  if(win) {
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

void io_encounter_pokemon()
{
  pokemon *p;

  p = new pokemon();

  clear();
  move(0,0);
  clrtoeol();
  printw("You encountered a wild %s!", p->get_species());
  // mvprintw(1, 0, "move 1: %s", p.move1.c_str());
  // mvprintw(2, 0, "move 2: %s", p.move2.c_str());
  // mvprintw(4, 0, "HP: %d ATK: %d DEF: %d SP-ATK: %d SP-DEF: %d SPE: %d", p.hp, p.atk, p.def, p.spa, p.spd, p.spe);
  // mvprintw(5, 0, "Press any key to leave");
  refresh();
  getch();

  // Later on, don't delete if captured
  delete p;
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
