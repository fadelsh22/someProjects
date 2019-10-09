#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>

#include "io.h"
#include "move.h"
#include "path.h"
#include "pc.h"
#include "utils.h"
#include "dungeon.h"
#include "object.h"
#include "npc.h"

/* Same ugly hack we did in path.c */
static dungeon *the_dungeon;

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void) {
  initscr();
  raw();
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

void io_reset_terminal(void) {
  endwin();

  while (io_head) {
	io_tail = io_head;
	io_head = io_head->next;
	free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...) {
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof(*tmp)))) {
	perror("malloc");
	exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof(tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
	io_head = io_tail = tmp;
  } else {
	io_tail->next = tmp;
	io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x) {
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

void io_display_tunnel(dungeon *d) {
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
	for (x = 0; x < DUNGEON_X; x++) {
	  if (charxy(x, y) == d->PC) {
		mvaddch(y + 1, x, charxy(x, y)->symbol);
	  } else if (hardnessxy(x, y) == 255) {
		mvaddch(y + 1, x, '*');
	  } else {
		mvaddch(y + 1, x, '0' + (d->pc_tunnel[y][x] % 10));
	  }
	}
  }
  refresh();
}

void io_display_distance(dungeon *d) {
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
	for (x = 0; x < DUNGEON_X; x++) {
	  if (charxy(x, y)) {
		mvaddch(y + 1, x, charxy(x, y)->symbol);
	  } else if (hardnessxy(x, y) != 0) {
		mvaddch(y + 1, x, ' ');
	  } else {
		mvaddch(y + 1, x, '0' + (d->pc_distance[y][x] % 10));
	  }
	}
  }
  refresh();
}

static char hardness_to_char[] =
	"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

void io_display_hardness(dungeon *d) {
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
	for (x = 0; x < DUNGEON_X; x++) {
	  mvaddch(y + 1, x, (d->hardness[y][x] ?
	                     hardness_to_char[1 + (int) ((d->hardness[y][x] /
		                     4.2))] : ' '));
	}
  }
  refresh();
}

/*char *io_object_to_string(object *o) {
}*/

static void io_redisplay_visible_monsters(dungeon *d) {
  /* This was initially supposed to only redisplay visible monsters.  After *
   * implementing that (comparitivly simple) functionality and testing, I   *
   * discovered that it resulted to dead monsters being displayed beyond    *
   * their lifetimes.  So it became necessary to implement the function for *
   * everything in the light radius.  In hindsight, it would be better to   *
   * artifacts), but it's still significantly slower than it could be.  I   *
   * will revisit this in the future to add the acceleration matrix.        */
  pair_t pos;
  uint32_t color;
  uint32_t illuminated;

  for (pos[dim_y] = -PC_VISUAL_RANGE;
       pos[dim_y] <= PC_VISUAL_RANGE;
       pos[dim_y]++) {
	for (pos[dim_x] = -PC_VISUAL_RANGE;
	     pos[dim_x] <= PC_VISUAL_RANGE;
	     pos[dim_x]++) {
	  if ((d->PC->position[dim_y] + pos[dim_y] < 0) ||
		  (d->PC->position[dim_y] + pos[dim_y] >= DUNGEON_Y) ||
		  (d->PC->position[dim_x] + pos[dim_x] < 0) ||
		  (d->PC->position[dim_x] + pos[dim_x] >= DUNGEON_X)) {
		continue;
	  }
	  if ((illuminated = is_illuminated(d->PC,
	                                    d->PC->position[dim_y] + pos[dim_y],
	                                    d->PC->position[dim_x] + pos[dim_x]))) {
		attron(A_BOLD);
	  }
	  if (d->character_map[d->PC->position[dim_y] + pos[dim_y]]
	  [d->PC->position[dim_x] + pos[dim_x]] &&
		  can_see(d, d->PC->position,
		          d->character_map[d->PC->position[dim_y] + pos[dim_y]]
		          [d->PC->position[dim_x] +
			          pos[dim_x]]->position, 1, 0)) {
		attron(COLOR_PAIR((color = d->character_map[d->PC->position[dim_y] +
			pos[dim_y]]
		[d->PC->position[dim_x] +
				pos[dim_x]]->get_color())));
		mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		        d->PC->position[dim_x] + pos[dim_x],
		        character_get_symbol(d->character_map[d->PC->position[dim_y] +
			        pos[dim_y]]
		                             [d->PC->position[dim_x] +
				        pos[dim_x]]));
		attroff(COLOR_PAIR(color));
	  } else if (d->objmap[d->PC->position[dim_y] + pos[dim_y]]
	  [d->PC->position[dim_x] + pos[dim_x]] &&
		  (can_see(d, d->PC->position,
		           d->objmap[d->PC->position[dim_y] + pos[dim_y]]
		           [d->PC->position[dim_x] +
			           pos[dim_x]]->get_position(), 1, 0) ||
			  d->objmap[d->PC->position[dim_y] + pos[dim_y]]
			  [d->PC->position[dim_x] + pos[dim_x]]->have_seen())) {
		attron(COLOR_PAIR(d->objmap[d->PC->position[dim_y] + pos[dim_y]]
		                  [d->PC->position[dim_x] +
				pos[dim_x]]->get_color()));
		mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		        d->PC->position[dim_x] + pos[dim_x],
		        d->objmap[d->PC->position[dim_y] + pos[dim_y]]
		        [d->PC->position[dim_x] + pos[dim_x]]->get_symbol());
		attroff(COLOR_PAIR(d->objmap[d->PC->position[dim_y] + pos[dim_y]]
		                   [d->PC->position[dim_x] +
				pos[dim_x]]->get_color()));
	  } else {
		switch (pc_learned_terrain(d->PC,
		                           d->PC->position[dim_y] + pos[dim_y],
		                           d->PC->position[dim_x] +
			                           pos[dim_x])) {
		  case ter_wall:
		  case ter_wall_immutable:
		  case ter_unknown:mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		                           d->PC->position[dim_x] + pos[dim_x], ' ');
			break;
		  case ter_floor:
		  case ter_floor_room:mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		                              d->PC->position[dim_x] + pos[dim_x], '.');
			break;
		  case ter_floor_hall:mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		                              d->PC->position[dim_x] + pos[dim_x], '#');
			break;
		  case ter_debug:mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		                         d->PC->position[dim_x] + pos[dim_x], '*');
			break;
		  case ter_stairs_up:mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		                             d->PC->position[dim_x] + pos[dim_x], '<');
			break;
		  case ter_stairs_down:mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
		                               d->PC->position[dim_x] + pos[dim_x], '>');
			break;
		  default:
			mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
			        d->PC->position[dim_x] + pos[dim_x], '0');
		}
	  }
	  attroff(A_BOLD);
	}
  }

  refresh();
}

static int compare_monster_distance(const void *v1, const void *v2) {
  const character *const *c1 = (const character *const *) v1;
  const character *const *c2 = (const character *const *) v2;

  return (the_dungeon->pc_distance[(*c1)->position[dim_y]]
  [(*c1)->position[dim_x]] -
	  the_dungeon->pc_distance[(*c2)->position[dim_y]]
	  [(*c2)->position[dim_x]]);
}

static character *io_nearest_visible_monster(dungeon *d) {
  character **c, *n;
  uint32_t x, y, count, i;

  c = (character **) malloc(d->num_monsters * sizeof(*c));

  /* Get a linear list of monsters */
  for (count = 0, y = 1; y < DUNGEON_Y - 1; y++) {
	for (x = 1; x < DUNGEON_X - 1; x++) {
	  if (d->character_map[y][x] && d->character_map[y][x] != d->PC) {
		c[count++] = d->character_map[y][x];
	  }
	}
  }
  the_dungeon = d;
  qsort(c, count, sizeof(*c), compare_monster_distance);

  for (n = NULL, i = 0; i < count; i++) {
	if (can_see(d, character_get_pos(d->PC), character_get_pos(c[i]), 1, 0)) {
	  n = c[i];
	  break;
	}
  }

  free(c);

  return n;
}

void io_display(dungeon *d) {
  pair_t pos;
  uint32_t illuminated;
  uint32_t color;
  character *c;
  int32_t visible_monsters;

  clear();
  for (visible_monsters = -1, pos[dim_y] = 0;
       pos[dim_y] < DUNGEON_Y;
       pos[dim_y]++) {
	for (pos[dim_x] = 0; pos[dim_x] < DUNGEON_X; pos[dim_x]++) {
	  if ((illuminated = is_illuminated(d->PC,
	                                    pos[dim_y],
	                                    pos[dim_x]))) {
		attron(A_BOLD);
	  }
	  if (d->character_map[pos[dim_y]]
	  [pos[dim_x]] &&
		  can_see(d,
		          character_get_pos(d->PC),
		          character_get_pos(d->character_map[pos[dim_y]]
		                            [pos[dim_x]]), 1, 0)) {
		visible_monsters++;
		attron(COLOR_PAIR((color = d->character_map[pos[dim_y]]
		[pos[dim_x]]->get_color())));
		mvaddch(pos[dim_y] + 1, pos[dim_x],
		        character_get_symbol(d->character_map[pos[dim_y]]
		                             [pos[dim_x]]));
		attroff(COLOR_PAIR(color));
	  } else if (d->objmap[pos[dim_y]]
	  [pos[dim_x]] &&
		  (d->objmap[pos[dim_y]]
		  [pos[dim_x]]->have_seen() ||
			  can_see(d, character_get_pos(d->PC), pos, 1, 0))) {
		attron(COLOR_PAIR(d->objmap[pos[dim_y]]
		                  [pos[dim_x]]->get_color()));
		mvaddch(pos[dim_y] + 1, pos[dim_x],
		        d->objmap[pos[dim_y]]
		        [pos[dim_x]]->get_symbol());
		attroff(COLOR_PAIR(d->objmap[pos[dim_y]]
		                   [pos[dim_x]]->get_color()));
	  } else {
		switch (pc_learned_terrain(d->PC,
		                           pos[dim_y],
		                           pos[dim_x])) {
		  case ter_wall:
		  case ter_wall_immutable:
		  case ter_unknown:mvaddch(pos[dim_y] + 1, pos[dim_x], ' ');
			break;
		  case ter_floor:
		  case ter_floor_room:mvaddch(pos[dim_y] + 1, pos[dim_x], '.');
			break;
		  case ter_floor_hall:mvaddch(pos[dim_y] + 1, pos[dim_x], '#');
			break;
		  case ter_debug:mvaddch(pos[dim_y] + 1, pos[dim_x], '*');
			break;
		  case ter_stairs_up:mvaddch(pos[dim_y] + 1, pos[dim_x], '<');
			break;
		  case ter_stairs_down:mvaddch(pos[dim_y] + 1, pos[dim_x], '>');
			break;
		  default:
			/* Use zero as an error symbol, since it stands out somewhat, and it's *
			 * not otherwise used.                                                 */
			mvaddch(pos[dim_y] + 1, pos[dim_x], '0');
		}
	  }
	  if (illuminated) {
		attroff(A_BOLD);
	  }
	}
  }

  mvprintw(23, 0, "PC position is (%3d,%2d).",
           character_get_x(d->PC), character_get_y(d->PC));

  mvprintw(22, 1, "%d known %s.", visible_monsters,
           visible_monsters > 1 ? "monsters" : "monster");
  if ((c = io_nearest_visible_monster(d))) {
	mvprintw(22, 30, "Nearest visible monster: %c at %d %c by %d %c.",
	         c->symbol,
	         abs(c->position[dim_y] - d->PC->position[dim_y]),
	         ((c->position[dim_y] - d->PC->position[dim_y]) <= 0 ?
	          'N' : 'S'),
	         abs(c->position[dim_x] - d->PC->position[dim_x]),
	         ((c->position[dim_x] - d->PC->position[dim_x]) <= 0 ?
	          'E' : 'W'));
  } else {
	mvprintw(22, 30, "Nearest visible monster: NONE.");
  }

  io_print_message_queue(0, 0);

  refresh();
}

static void io_redisplay_non_terrain(dungeon *d, pair_t cursor) {
  /* For the wiz-mode teleport, in order to see color-changing effects. */
  pair_t pos;
  uint32_t color;
  uint32_t illuminated;

  for (pos[dim_y] = 0; pos[dim_y] < DUNGEON_Y; pos[dim_y]++) {
	for (pos[dim_x] = 0; pos[dim_x] < DUNGEON_X; pos[dim_x]++) {
	  if ((illuminated = is_illuminated(d->PC,
	                                    pos[dim_y],
	                                    pos[dim_x]))) {
		attron(A_BOLD);
	  }
	  if (cursor[dim_y] == pos[dim_y] && cursor[dim_x] == pos[dim_x]) {
		mvaddch(pos[dim_y] + 1, pos[dim_x], '*');
	  } else if (d->character_map[pos[dim_y]][pos[dim_x]]) {
		attron(COLOR_PAIR((color = d->character_map[pos[dim_y]]
		[pos[dim_x]]->get_color())));
		mvaddch(pos[dim_y] + 1, pos[dim_x],
		        character_get_symbol(d->character_map[pos[dim_y]][pos[dim_x]]));
		attroff(COLOR_PAIR(color));
	  } else if (d->objmap[pos[dim_y]][pos[dim_x]]) {
		attron(COLOR_PAIR(d->objmap[pos[dim_y]][pos[dim_x]]->get_color()));
		mvaddch(pos[dim_y] + 1, pos[dim_x],
		        d->objmap[pos[dim_y]][pos[dim_x]]->get_symbol());
		attroff(COLOR_PAIR(d->objmap[pos[dim_y]][pos[dim_x]]->get_color()));
	  }
	  attroff(A_BOLD);
	}
  }

  refresh();
}

void io_display_no_fog(dungeon *d) {
  uint32_t y, x;
  uint32_t color;
  character *c;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
	for (x = 0; x < DUNGEON_X; x++) {
	  if (d->character_map[y][x]) {
		attron(COLOR_PAIR((color = d->character_map[y][x]->get_color())));
		mvaddch(y + 1, x, character_get_symbol(d->character_map[y][x]));
		attroff(COLOR_PAIR(color));
	  } else if (d->objmap[y][x]) {
		attron(COLOR_PAIR(d->objmap[y][x]->get_color()));
		mvaddch(y + 1, x, d->objmap[y][x]->get_symbol());
		attroff(COLOR_PAIR(d->objmap[y][x]->get_color()));
	  } else {
		switch (mapxy(x, y)) {
		  case ter_wall:
		  case ter_wall_immutable:mvaddch(y + 1, x, ' ');
			break;
		  case ter_floor:
		  case ter_floor_room:mvaddch(y + 1, x, '.');
			break;
		  case ter_floor_hall:mvaddch(y + 1, x, '#');
			break;
		  case ter_debug:mvaddch(y + 1, x, '*');
			break;
		  case ter_stairs_up:mvaddch(y + 1, x, '<');
			break;
		  case ter_stairs_down:mvaddch(y + 1, x, '>');
			break;
		  default:
			/* Use zero as an error symbol, since it stands out somewhat, and it's *
			 * not otherwise used.                                                 */
			mvaddch(y + 1, x, '0');
		}
	  }
	}
  }

  mvprintw(23, 1, "PC position is (%2d,%2d).",
           d->PC->position[dim_x], d->PC->position[dim_y]);
  mvprintw(22, 1, "%d known %s.", d->num_monsters,
           d->num_monsters > 1 ? "monsters" : "monster");
  if ((c = io_nearest_visible_monster(d))) {
	mvprintw(22, 30, "Nearest visible monster: %c at %d %c by %d %c.",
	         c->symbol,
	         abs(c->position[dim_y] - d->PC->position[dim_y]),
	         ((c->position[dim_y] - d->PC->position[dim_y]) <= 0 ?
	          'N' : 'S'),
	         abs(c->position[dim_x] - d->PC->position[dim_x]),
	         ((c->position[dim_x] - d->PC->position[dim_x]) <= 0 ?
	          'E' : 'W'));
  } else {
	mvprintw(22, 30, "Nearest visible monster: NONE.");
  }

  io_print_message_queue(0, 0);
  refresh();
}

void io_display_monster_list(dungeon *d) {
  mvprintw(11, 33, " HP:    XXXXX ");
  mvprintw(12, 33, " Speed: XXXXX ");
  mvprintw(14, 27, " Hit any key to continue. ");
  refresh();
  getch();
}

uint32_t io_teleport_pc(dungeon *d) {
  pair_t dest;
  int c;
  fd_set readfs;
  struct timeval tv;
  io_display_no_fog(d);

  mvprintw(0, 0, "Choose a location.  't' to teleport to; 'r' for random.");

  dest[dim_y] = d->PC->position[dim_y];
  dest[dim_x] = d->PC->position[dim_x];

  mvaddch(dest[dim_y] + 1, dest[dim_x], '*');
  refresh();

  do {
	do {
	  FD_ZERO(&readfs);
	  FD_SET(STDIN_FILENO, &readfs);

	  tv.tv_sec = 0;
	  tv.tv_usec = 125000; /* An eigth of a second */

	  io_redisplay_non_terrain(d, dest);
	} while (!select(STDIN_FILENO + 1, &readfs, NULL, NULL, &tv));
	switch (mappair(dest)) {
	  case ter_wall:
	  case ter_wall_immutable:
	  case ter_unknown:mvaddch(dest[dim_y] + 1, dest[dim_x], ' ');
		break;
	  case ter_floor:
	  case ter_floor_room:mvaddch(dest[dim_y] + 1, dest[dim_x], '.');
		break;
	  case ter_floor_hall:mvaddch(dest[dim_y] + 1, dest[dim_x], '#');
		break;
	  case ter_debug:mvaddch(dest[dim_y] + 1, dest[dim_x], '*');
		break;
	  case ter_stairs_up:mvaddch(dest[dim_y] + 1, dest[dim_x], '<');
		break;
	  case ter_stairs_down:mvaddch(dest[dim_y] + 1, dest[dim_x], '>');
		break;
	  default:
		/* Use zero as an error symbol, since it stands out somewhat, and it's *
		 * not otherwise used.                                                 */
		mvaddch(dest[dim_y] + 1, dest[dim_x], '0');
	}
	switch ((c = getch())) {
	  case '7':
	  case 'y':
	  case KEY_HOME:
		if (dest[dim_y] != 1) {
		  dest[dim_y]--;
		}
		if (dest[dim_x] != 1) {
		  dest[dim_x]--;
		}
		break;
	  case '8':
	  case 'k':
	  case KEY_UP:
		if (dest[dim_y] != 1) {
		  dest[dim_y]--;
		}
		break;
	  case '9':
	  case 'u':
	  case KEY_PPAGE:
		if (dest[dim_y] != 1) {
		  dest[dim_y]--;
		}
		if (dest[dim_x] != DUNGEON_X - 2) {
		  dest[dim_x]++;
		}
		break;
	  case '6':
	  case 'l':
	  case KEY_RIGHT:
		if (dest[dim_x] != DUNGEON_X - 2) {
		  dest[dim_x]++;
		}
		break;
	  case '3':
	  case 'n':
	  case KEY_NPAGE:
		if (dest[dim_y] != DUNGEON_Y - 2) {
		  dest[dim_y]++;
		}
		if (dest[dim_x] != DUNGEON_X - 2) {
		  dest[dim_x]++;
		}
		break;
	  case '2':
	  case 'j':
	  case KEY_DOWN:
		if (dest[dim_y] != DUNGEON_Y - 2) {
		  dest[dim_y]++;
		}
		break;
	  case '1':
	  case 'b':
	  case KEY_END:
		if (dest[dim_y] != DUNGEON_Y - 2) {
		  dest[dim_y]++;
		}
		if (dest[dim_x] != 1) {
		  dest[dim_x]--;
		}
		break;
	  case '4':
	  case 'h':
	  case KEY_LEFT:
		if (dest[dim_x] != 1) {
		  dest[dim_x]--;
		}
		break;
	}
  } while (c != 't' && c != 'r');

  if (c == 'r') {
	do {
	  dest[dim_x] = rand_range(1, DUNGEON_X - 2);
	  dest[dim_y] = rand_range(1, DUNGEON_Y - 2);
	} while (charpair(dest) || mappair(dest) < ter_floor);
  }

  if (charpair(dest) && charpair(dest) != d->PC) {
	io_queue_message("Teleport failed.  Destination occupied.");
  } else {
	d->character_map[d->PC->position[dim_y]][d->PC->position[dim_x]] = NULL;
	d->character_map[dest[dim_y]][dest[dim_x]] = d->PC;

	d->PC->position[dim_y] = dest[dim_y];
	d->PC->position[dim_x] = dest[dim_x];
  }

  pc_observe_terrain(d->PC, d);
  dijkstra(d);
  dijkstra_tunnel(d);

  io_display(d);

  return 0;
}

#if o
/* Adjectives to describe our monsters */
static const char *adjectives[] = {
  "A menacing ",
  "A threatening ",
  "A horrifying ",
  "An intimidating ",
  "An aggressive ",
  "A frightening ",
  "A terrifying ",
  "A terrorizing ",
  "An alarming ",
  "A frightening ",
  "A dangerous ",
  "A glowering ",
  "A glaring ",
  "A scowling ",
  "A chilling ",
  "A scary ",
  "A creepy ",
  "An eerie ",
  "A spooky ",
  "A slobbering ",
  "A drooling ",
  " A horrendous ",
  "An unnerving ",
  "A cute little ",  /* Even though they're trying to kill you, */
  "A teeny-weenie ", /* they can still be cute!                 */
  "A fuzzy ",
  "A fluffy white ",
  "A kawaii ",       /* For our otaku */
  "Hao ke ai de "    /* And for our Chinese */
  /* And there's one special case (see below) */
};
#endif

static void io_scroll_monster_list(char (*s)[40], uint32_t count) {
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
	  case 27:return;
	}
  }
}

static bool is_vowel(const char c) {
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
	  c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

static void io_list_monsters_display(dungeon *d,
                                     character **c,
                                     uint32_t count) {
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc((count + 1) * sizeof(*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d monsters:", count);
  mvprintw(4, 19, " %-40s ", s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
	snprintf(s[i], 40, "%3s%s (%c): %2d %s by %2d %s",
	         (is_unique(c[i]) ? "" :
	          (is_vowel(character_get_name(c[i])[0]) ? "An " : "A ")),
	         character_get_name(c[i]),
	         character_get_symbol(c[i]),
	         abs(character_get_y(c[i]) - character_get_y(d->PC)),
	         ((character_get_y(c[i]) - character_get_y(d->PC)) <= 0 ?
	          "North" : "South"),
	         abs(character_get_x(c[i]) - character_get_x(d->PC)),
	         ((character_get_x(c[i]) - character_get_x(d->PC)) <= 0 ?
	          "East" : "West"));
	if (count <= 13) {
	  /* Handle the non-scrolling case right here. *
	   * Scrolling in another function.            */
	  mvprintw(i + 6, 19, " %-40s ", s[i]);
	}
  }

  if (count <= 13) {
	mvprintw(count + 6, 19, " %-40s ", "");
	mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
	while (getch() != 27 /* escape */);
  } else {
	mvprintw(19, 19, " %-40s ", "");
	mvprintw(20, 19, " %-40s ",
	         "Arrows to scroll, escape to continue.");
	io_scroll_monster_list(s, count);
  }

  free(s);
}

static void io_list_monsters(dungeon *d) {
  character **c;
  uint32_t x, y, count;

  c = (character **) malloc(d->num_monsters * sizeof(*c));

  /* Get a linear list of monsters */
  for (count = 0, y = 1; y < DUNGEON_Y - 1; y++) {
	for (x = 1; x < DUNGEON_X - 1; x++) {
	  if (d->character_map[y][x] && d->character_map[y][x] != d->PC &&
		  can_see(d, character_get_pos(d->PC),
		          character_get_pos(d->character_map[y][x]), 1, 0)) {
		c[count++] = d->character_map[y][x];
	  }
	}
  }

  the_dungeon = d;
  qsort(c, count, sizeof(*c), compare_monster_distance);
  io_list_monsters_display(d, c, count);
  free(c);
  io_display(d);
}

void organize_inventory(dungeon *d) {
  for (int i = 0; i < NUM_INVENTORY - 1; ++i) {
	if (d->PC->inventory[i] == NULL && d->PC->inventory[i + 1] != NULL) {
	  d->PC->inventory[i] = d->PC->inventory[i + 1];
	  d->PC->inventory[i + 1] = NULL;
	}
  }
}

void clear_screen() {
  for (int i = 0; i < DUNGEON_Y + 2; ++i) {
	for (int j = 0; j < DUNGEON_X + 2; ++j) {
	  mvaddch(i, j, ' ');
	}
  }
}

void put_in_inv(dungeon *d, int desired_object) {
  int is_full = 1;
  for (int i = 0; i < NUM_INVENTORY; ++i) {
	if (d->PC->inventory[i] == NULL) { 
	  d->PC->inventory[i] = d->PC->equipped[desired_object];
	  d->PC->equipped[desired_object] = NULL;
	  is_full = 0;
	}
  }
  if (is_full == 1) {
	io_queue_message("Your inventory is full. Drop or expunge this item to unequip.");
  }
}

void display_inv_labels(dungeon *d) {
  if (d->PC->inventory[0] == NULL) {
	mvprintw(6, 15, "0)");
  } else {
	mvprintw(6, 15, "0)");
	mvprintw(6, 18, d->PC->inventory[0]->get_name());
  }
  if (d->PC->inventory[1] == NULL) {
	mvprintw(7, 15, "1)");
  } else {
	mvprintw(7, 15, "1)");
	mvprintw(7, 18, d->PC->inventory[1]->get_name());
  }
  if (d->PC->inventory[2] == NULL) {
	mvprintw(8, 15, "2)");
  } else {
	mvprintw(8, 15, "2)");
	mvprintw(8, 18, d->PC->inventory[2]->get_name());
  }
  if (d->PC->inventory[3] == NULL) {
	mvprintw(9, 15, "3)");
  } else {
	mvprintw(9, 15, "3)");
	mvprintw(9, 18, d->PC->inventory[3]->get_name());
  }
  if (d->PC->inventory[4] == NULL) {
	mvprintw(10, 15, "4)");
  } else {
	mvprintw(10, 15, "4)");
	mvprintw(10, 18, d->PC->inventory[4]->get_name());
  }
  if (d->PC->inventory[5] == NULL) {
	mvprintw(11, 15, "5)");
  } else {
	mvprintw(11, 15, "5)");
	mvprintw(11, 18, d->PC->inventory[5]->get_name());
  }
  if (d->PC->inventory[6] == NULL) {
	mvprintw(12, 15, "6)");
  } else {
	mvprintw(12, 15, "6)");
	mvprintw(12, 18, d->PC->inventory[6]->get_name());
  }
  if (d->PC->inventory[7] == NULL) {
	mvprintw(13, 15, "7)");
  } else {
	mvprintw(13, 15, "7)");
	mvprintw(13, 18, d->PC->inventory[7]->get_name());
  }
  if (d->PC->inventory[8] == NULL) {
	mvprintw(14, 15, "8)");
  } else {
	mvprintw(14, 15, "8)");
	mvprintw(14, 18, d->PC->inventory[8]->get_name());
  }
  if (d->PC->inventory[9] == NULL) {
	mvprintw(15, 15, "9)");
  } else {
	mvprintw(15, 15, "9)");
	mvprintw(15, 18, d->PC->inventory[9]->get_name());
  }
}

void display_equip_labels(dungeon *d) {
  if (d->PC->equipped[weapon] == NULL) {
	mvprintw(6, 15, "a)");
  } else {
	mvprintw(6, 15, "a)");
	mvprintw(6, 18, d->PC->equipped[weapon]->get_name());
  }
  if (d->PC->equipped[offhand] == NULL) {
	mvprintw(7, 15, "b)");
  } else {
	mvprintw(7, 15, "b)");
	mvprintw(7, 18, d->PC->equipped[offhand]->get_name());
  }
  if (d->PC->equipped[ranged] == NULL) {
	mvprintw(8, 15, "c)");
  } else {
	mvprintw(8, 15, "c)");
	mvprintw(8, 18, d->PC->equipped[ranged]->get_name());
  }
  if (d->PC->equipped[armor] == NULL) {
	mvprintw(9, 15, "d)");
  } else {
	mvprintw(9, 15, "d)");
	mvprintw(9, 18, d->PC->equipped[armor]->get_name());
  }
  if (d->PC->equipped[helmet] == NULL) {
	mvprintw(10, 15, "e)");
  } else {
	mvprintw(10, 15, "e)");
	mvprintw(10, 18, d->PC->equipped[helmet]->get_name());
  }
  if (d->PC->equipped[cloak] == NULL) {
	mvprintw(11, 15, "f)");
  } else {
	mvprintw(11, 15, "f)");
	mvprintw(11, 18, d->PC->equipped[cloak]->get_name());
  }
  if (d->PC->equipped[gloves] == NULL) {
	mvprintw(12, 15, "g)");
  } else {
	mvprintw(12, 15, "g)");
	mvprintw(12, 18, d->PC->equipped[gloves]->get_name());
  }
  if (d->PC->equipped[boots] == NULL) {
	mvprintw(13, 15, "h)");
  } else {
	mvprintw(13, 15, "h)");
	mvprintw(13, 18, d->PC->equipped[boots]->get_name());
  }
  if (d->PC->equipped[amulet] == NULL) {
	mvprintw(14, 15, "i)");
  } else {
	mvprintw(14, 15, "i)");
	mvprintw(14, 18, d->PC->equipped[amulet]->get_name());
  }
  if (d->PC->equipped[light] == NULL) {
	mvprintw(15, 15, "j)");
  } else {
	mvprintw(15, 15, "j)");
	mvprintw(15, 18, d->PC->equipped[amulet]->get_name());
  }
  if (d->PC->equipped[light] == NULL) {
	mvprintw(16, 15, "k)");
  } else {
	mvprintw(16, 15, "k)");
	mvprintw(16, 18, d->PC->equipped[light]->get_name());
  }
  if (d->PC->equipped[ring_one] == NULL) {
	mvprintw(17, 15, "l)");
  } else {
	mvprintw(17, 15, "l)");
	mvprintw(17, 18, d->PC->equipped[ring_one]->get_name());
  }
  if (d->PC->equipped[ring_two] == NULL) {
	mvprintw(18, 15, "1)");
  } else {
	mvprintw(18, 15, "1)");
	mvprintw(18, 18, d->PC->equipped[ring_two]->get_name());
  }
}
void wear_equipment(dungeon_t *d, int invent_slot) {
  if (d->PC->inventory[invent_slot]->get_type() == objtype_WEAPON) {
	if (d->PC->equipped[weapon] == NULL) { 
	  d->PC->equipped[weapon] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[weapon];
	  d->PC->equipped[weapon] = temp;
	}
	d->PC->speed += d->PC->equipped[weapon]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_OFFHAND) {
	if (d->PC->equipped[offhand] == NULL) { 
	  d->PC->equipped[offhand] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[offhand];
	  d->PC->equipped[offhand] = temp;
	}
	d->PC->speed += d->PC->equipped[offhand]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_RANGED) {
	if (d->PC->equipped[ranged] == NULL) { 
	  d->PC->equipped[ranged] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[ranged];
	  d->PC->equipped[ranged] = temp;
	}
	d->PC->speed += d->PC->equipped[ranged]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_ARMOR) {
	if (d->PC->equipped[armor] == NULL) { 
	  d->PC->equipped[armor] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[armor];
	  d->PC->equipped[armor] = temp;
	}
	d->PC->speed += d->PC->equipped[armor]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_HELMET) {
	if (d->PC->equipped[helmet] == NULL) { 
	  d->PC->equipped[helmet] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[helmet];
	  d->PC->equipped[helmet] = temp;
	}
	d->PC->speed += d->PC->equipped[helmet]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_CLOAK) {
	if (d->PC->equipped[cloak] == NULL) { 
	  d->PC->equipped[cloak] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[cloak];
	  d->PC->equipped[cloak] = temp;
	}
	d->PC->speed += d->PC->equipped[cloak]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_GLOVES) {
	if (d->PC->equipped[gloves] == NULL) { 
	  d->PC->equipped[gloves] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[gloves];
	  d->PC->equipped[gloves] = temp;
	}
	d->PC->speed += d->PC->equipped[gloves]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_BOOTS) {
	if (d->PC->equipped[boots] == NULL) { 
	  d->PC->equipped[boots] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[boots];
	  d->PC->equipped[boots] = temp;
	}
	d->PC->speed += d->PC->equipped[boots]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_AMULET) {
	if (d->PC->equipped[amulet] == NULL) { 
	  d->PC->equipped[amulet] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[amulet];
	  d->PC->equipped[amulet] = temp;
	}
	d->PC->speed += d->PC->equipped[amulet]->get_speed();
  } else if (d->PC->inventory[invent_slot]->get_type() == objtype_LIGHT) {
	if (d->PC->equipped[light] == NULL) { 
	  d->PC->equipped[light] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	} else { 
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[light];
	  d->PC->equipped[light] = temp;
	}
	d->PC->speed += d->PC->equipped[light]->get_speed();
  } else { 
	if (d->PC->equipped[ring_one] == NULL) { 
	  d->PC->equipped[ring_one] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	  d->PC->speed += d->PC->equipped[ring_one]->get_speed();
	} else if (d->PC->equipped[ring_two] == NULL) {
	  d->PC->equipped[ring_two] = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = NULL;
	  organize_inventory(d);
	  d->PC->speed += d->PC->equipped[ring_two]->get_speed();
	} else {
	  object *temp = d->PC->inventory[invent_slot];
	  d->PC->inventory[invent_slot] = d->PC->equipped[ring_two];
	  d->PC->equipped[ring_two] = temp;
	  d->PC->speed += d->PC->equipped[ring_two]->get_speed();
	}

  }
}

void io_display_inv(dungeon_t *d) {
  clear_screen();
  mvprintw(4, 15, "INVENTORY");
  display_inv_labels(d);

  const char
	  *option_one = "[w]ear an item, [d]rop an item, e[x]punge an item, [I]nspect an item";
  const char *option_two = "Or, press any key to exit inventory";
  mvprintw(20, 5, option_one);
  mvprintw(21, 5, option_two);

  switch (int input = getch()) {
	case 'w':mvprintw(20, 5, "Choose an item to equip                                                  ");
	  switch (input = getch()) { 
		case '0':
		  if (d->PC->inventory[0] != NULL) {
			wear_equipment(d, 0);
		  }
		  break;
		case '1':
		  if (d->PC->inventory[1] != NULL) {
			wear_equipment(d, 1);
		  }
		  break;
		case '2':
		  if (d->PC->inventory[2] != NULL) {
			wear_equipment(d, 2);
		  }
		  break;
		case '3':
		  if (d->PC->inventory[3] != NULL) {
			wear_equipment(d, 3);
		  }
		  break;
		case '4':
		  if (d->PC->inventory[4] != NULL) {
			wear_equipment(d, 4);
		  }
		  break;
		case '5':
		  if (d->PC->inventory[5] != NULL) {
			wear_equipment(d, 5);
		  }
		  break;
		case '6':
		  if (d->PC->inventory[6] != NULL) {
			wear_equipment(d, 6);
		  }
		  break;
		case '7':
		  if (d->PC->inventory[7] != NULL) {
			wear_equipment(d, 7);
		  }
		  break;
		case '8':
		  if (d->PC->inventory[8] != NULL) {
			wear_equipment(d, 8);
		  }
		  break;
		case '9':
		  if (d->PC->inventory[8] != NULL) {
			wear_equipment(d, 8);
		  }
		  break;
		default: break;
	  }
	  break;
	case 'd': 
	  mvprintw(20, 5, "Choose an item to drop                                                  ");
	  switch (input = getch()) {
		case '0':
		  if (d->PC->inventory[0] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[0];
			d->PC->inventory[0] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '1':
		  if (d->PC->inventory[1] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[1];
			d->PC->inventory[1] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '2':
		  if (d->PC->inventory[2] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[2];
			d->PC->inventory[2] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '3':
		  if (d->PC->inventory[3] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[3];
			d->PC->inventory[3] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '4':
		  if (d->PC->inventory[4] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[4];
			d->PC->inventory[4] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '5':
		  if (d->PC->inventory[5] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[5];
			d->PC->inventory[5] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '6':
		  if (d->PC->inventory[6] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[6];
			d->PC->inventory[6] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '7':
		  if (d->PC->inventory[7] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[7];
			d->PC->inventory[7] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '8':
		  if (d->PC->inventory[8] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[8];
			d->PC->inventory[8] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '9':
		  if (d->PC->inventory[9] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->inventory[9];
			d->PC->inventory[9] = NULL;
			organize_inventory(d);
		  }
		  break;
		default: break;
	  }
	  break;
	case 'x': 
	  mvprintw(20, 5, "Choose an item to delete                                                  ");
	  switch (input = getch()) {
		case '0':
		  if (d->PC->inventory[0] != NULL) {
			delete (d->PC->inventory[0]);
			d->PC->inventory[0] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '1':
		  if (d->PC->inventory[1] != NULL) {
			delete (d->PC->inventory[1]);
			d->PC->inventory[1] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '2':
		  if (d->PC->inventory[2] != NULL) {
			delete (d->PC->inventory[2]);
			d->PC->inventory[2] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '3':
		  if (d->PC->inventory[3] != NULL) {
			delete (d->PC->inventory[3]);
			d->PC->inventory[3] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '4':
		  if (d->PC->inventory[4] != NULL) {
			delete (d->PC->inventory[4]);
			d->PC->inventory[4] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '5':
		  if (d->PC->inventory[5] != NULL) {
			delete (d->PC->inventory[5]);
			d->PC->inventory[5] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '6':
		  if (d->PC->inventory[6] != NULL) {
			delete (d->PC->inventory[6]);
			d->PC->inventory[6] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '7':
		  if (d->PC->inventory[7] != NULL) {
			delete (d->PC->inventory[7]);
			d->PC->inventory[7] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '8':
		  if (d->PC->inventory[8] != NULL) {
			delete (d->PC->inventory[8]);
			d->PC->inventory[8] = NULL;
			organize_inventory(d);
		  }
		  break;
		case '9':
		  if (d->PC->inventory[9] != NULL) {
			delete (d->PC->inventory[9]);
			d->PC->inventory[9] = NULL;
			organize_inventory(d);
		  }
		  break;
		default: break;
	  }
	  break;
	case 'I': 
	  mvprintw(20, 5, "Choose an item to inspect                                                  ");
	  switch (input = getch()) {
		case '0':
		  if (d->PC->inventory[0] != NULL) {
			examine_item(d->PC->inventory[0]);
		  }
		  break;
		case '1':
		  if (d->PC->inventory[1] != NULL) {
			examine_item(d->PC->inventory[1]);
		  }
		  break;
		case '2':
		  if (d->PC->inventory[2] != NULL) {
			examine_item(d->PC->inventory[2]);
		  }
		  break;
		case '3':
		  if (d->PC->inventory[3] != NULL) {
			examine_item(d->PC->inventory[3]);
		  }
		  break;
		case '4':
		  if (d->PC->inventory[4] != NULL) {
			examine_item(d->PC->inventory[4]);
		  }
		  break;
		case '5':
		  if (d->PC->inventory[5] != NULL) {
			examine_item(d->PC->inventory[5]);
		  }
		  break;
		case '6':
		  if (d->PC->inventory[6] != NULL) {
			examine_item(d->PC->inventory[6]);
		  }
		  break;
		case '7':
		  if (d->PC->inventory[7] != NULL) {
			examine_item(d->PC->inventory[7]);
		  }
		  break;
		case '8':
		  if (d->PC->inventory[8] != NULL) {
			examine_item(d->PC->inventory[8]);
		  }
		  break;
		case '9':
		  if (d->PC->inventory[9] != NULL) {
			examine_item(d->PC->inventory[9]);
		  }
		  break;
		default: break;
	  }
	  break;
	default:break;
  }
  clear_screen();
}

void io_display_eq(dungeon_t *d) {
  clear_screen();
  mvprintw(4, 15, "EQUIPMENT");
  display_equip_labels(d);
  const char
	  *option_one = "[t]ake off an item, [d]rop an item, e[x]punge an item, [I]nspect an item";
  const char *option_two = "Or, press any key to exit equipment";
  mvprintw(20, 5, option_one);
  mvprintw(21, 5, option_two);
  int input = 0;
  switch (input = getch()) {
	case 't': 
	  mvprintw(20, 5, "Choose an item to take off                                                  ");
	  switch (input = getch()) {
		case 'a':
		  if (d->PC->equipped[weapon] != NULL) {
			d->PC->speed -= d->PC->equipped[weapon]->get_speed(); 
			put_in_inv(d, weapon);
		  }
		  break;
		case 'b':
		  if (d->PC->equipped[offhand] != NULL) {
			d->PC->speed -= d->PC->equipped[offhand]->get_speed(); 
			put_in_inv(d, offhand);
		  }
		  break;
		case 'c':
		  if (d->PC->equipped[ranged] != NULL) {
			d->PC->speed -= d->PC->equipped[ranged]->get_speed(); 
			put_in_inv(d, ranged);
		  }
		  break;
		case 'd':
		  if (d->PC->equipped[armor] != NULL) {
			d->PC->speed -= d->PC->equipped[armor]->get_speed(); 
			put_in_inv(d, armor);
		  }
		  break;
		case 'e':
		  if (d->PC->equipped[helmet] != NULL) {
			d->PC->speed -= d->PC->equipped[helmet]->get_speed(); 
			put_in_inv(d, helmet);
		  }
		  break;
		case 'f':
		  if (d->PC->equipped[cloak] != NULL) {
			d->PC->speed -= d->PC->equipped[cloak]->get_speed(); 
			put_in_inv(d, cloak);
		  }
		  break;
		case 'g':
		  if (d->PC->equipped[gloves] != NULL) {
			d->PC->speed -= d->PC->equipped[gloves]->get_speed(); 
			put_in_inv(d, gloves);
		  }
		  break;
		case 'h':
		  if (d->PC->equipped[boots] != NULL) {
			d->PC->speed -= d->PC->equipped[boots]->get_speed(); 
			put_in_inv(d, boots);
		  }
		  break;
		case 'i':
		  if (d->PC->equipped[amulet] != NULL) {
			d->PC->speed -= d->PC->equipped[amulet]->get_speed(); 
			put_in_inv(d, amulet);
		  }
		  break;
		case 'j':
		  if (d->PC->equipped[light] != NULL) {
			d->PC->speed -= d->PC->equipped[light]->get_speed(); 
			put_in_inv(d, light);
		  }
		  break;
		case 'k':
		  if (d->PC->equipped[ring_one] != NULL) {
			d->PC->speed -= d->PC->equipped[ring_one]->get_speed(); 
			put_in_inv(d, ring_one);
		  }
		  break;
		case '1':
		  if (d->PC->equipped[ring_two] != NULL) {
			d->PC->speed -= d->PC->equipped[ring_two]->get_speed(); 
			put_in_inv(d, ring_two);
		  }
		  break;
		default: break;
	  }
	  break;
	case 'd': 
	  mvprintw(20, 5, "Choose an item to drop                                                  ");
	  switch (input = getch()) {
		case 'a':
		  if (d->PC->equipped[weapon] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[weapon];
			d->PC->equipped[weapon] = NULL;
		  }
		  break;
		case 'b':
		  if (d->PC->equipped[offhand] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[offhand];
			d->PC->equipped[offhand] = NULL;
		  }
		  break;
		case 'c':
		  if (d->PC->equipped[ranged] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[ranged];
			d->PC->equipped[ranged] = NULL;
		  }
		  break;
		case 'd':
		  if (d->PC->equipped[armor] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[armor];
			d->PC->equipped[armor] = NULL;
		  }
		  break;
		case 'e':
		  if (d->PC->equipped[helmet] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[helmet];
			d->PC->equipped[helmet] = NULL;
		  }
		  break;
		case 'f':
		  if (d->PC->equipped[cloak] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[cloak];
			d->PC->equipped[cloak] = NULL;
		  }
		  break;
		case 'g':
		  if (d->PC->equipped[gloves] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[cloak];
			d->PC->equipped[gloves] = NULL;
		  }
		  break;
		case 'h':
		  if (d->PC->equipped[boots] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[boots];
			d->PC->equipped[boots] = NULL;
		  }
		  break;
		case 'i':
		  if (d->PC->equipped[amulet] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[amulet];
			d->PC->equipped[amulet] = NULL;
		  }
		  break;
		case 'j':
		  if (d->PC->equipped[light] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[light];
			d->PC->equipped[light] = NULL;
		  }
		  break;
		case 'k':
		  if (d->PC->equipped[ring_one] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[ring_one];
			d->PC->equipped[ring_one] = NULL;
		  }
		  break;
		case '1':
		  if (d->PC->equipped[ring_two] != NULL) {
			d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC->equipped[ring_two];
			d->PC->equipped[ring_two] = NULL;
		  }
		  break;
		default: break;
	  }
	  break;
	case 'x': 
	  mvprintw(20, 5, "Choose an item to delete                                                  ");
	  switch (input = getch()) {
		case 'a':
		  if (d->PC->equipped[weapon] != NULL) {
			delete (d->PC->equipped[weapon]);
			d->PC->equipped[weapon] = NULL;
		  }
		  break;
		case 'b':
		  if (d->PC->equipped[offhand] != NULL) {
			delete (d->PC->equipped[offhand]);
			d->PC->equipped[offhand] = NULL;
		  }
		  break;
		case 'c':
		  if (d->PC->equipped[ranged] != NULL) {
			delete (d->PC->equipped[ranged]);
			d->PC->equipped[ranged] = NULL;
		  }
		  break;
		case 'd':
		  if (d->PC->equipped[armor] != NULL) {
			delete (d->PC->equipped[armor]);
			d->PC->equipped[armor] = NULL;
		  }
		  break;
		case 'e':
		  if (d->PC->equipped[helmet] != NULL) {
			delete (d->PC->equipped[helmet]);
			d->PC->equipped[helmet] = NULL;
		  }
		  break;
		case 'f':
		  if (d->PC->equipped[cloak] != NULL) {
			delete (d->PC->equipped[cloak]);
			d->PC->equipped[cloak] = NULL;
		  }
		  break;
		case 'g':
		  if (d->PC->equipped[gloves] != NULL) {
			delete (d->PC->equipped[gloves]);
			d->PC->equipped[gloves] = NULL;
		  }
		  break;
		case 'h':
		  if (d->PC->equipped[boots] != NULL) {
			delete (d->PC->equipped[boots]);
			d->PC->equipped[boots] = NULL;
		  }
		  break;
		case 'i':
		  if (d->PC->equipped[amulet] != NULL) {
			delete (d->PC->equipped[amulet]);
			d->PC->equipped[amulet] = NULL;
		  }
		  break;
		case 'j':
		  if (d->PC->equipped[light] != NULL) {
			delete (d->PC->equipped[light]);
			d->PC->equipped[light] = NULL;
		  }
		  break;
		case 'k':
		  if (d->PC->equipped[ring_one] != NULL) {
			delete (d->PC->equipped[ring_one]);
			d->PC->equipped[ring_one] = NULL;
		  }
		  break;
		case '1':
		  if (d->PC->equipped[ring_two] != NULL) {
			delete (d->PC->equipped[ring_two]);
			d->PC->equipped[ring_two] = NULL;
		  }
		  break;
		default: break;
	  }
	  break;
	case 'I': 
	  mvprintw(20, 5, "Choose an item to inspect                                                ");
	  switch (input = getch()) {
		case 'a':
		  if (d->PC->equipped[weapon] != NULL) {
			examine_item(d->PC->equipped[weapon]);
		  }
		  break;
		case 'b':
		  if (d->PC->equipped[offhand] != NULL) {
			examine_item(d->PC->equipped[offhand]);
		  }
		  break;
		case 'c':
		  if (d->PC->equipped[ranged] != NULL) {
			examine_item(d->PC->equipped[ranged]);
		  }
		  break;
		case 'd':
		  if (d->PC->equipped[armor] != NULL) {
			examine_item(d->PC->equipped[armor]);
		  }
		  break;
		case 'e':
		  if (d->PC->equipped[helmet] != NULL) {
			examine_item(d->PC->equipped[helmet]);
		  }
		  break;
		case 'f':
		  if (d->PC->equipped[cloak] != NULL) {
			examine_item(d->PC->equipped[cloak]);
		  }
		  break;
		case 'g':
		  if (d->PC->equipped[gloves] != NULL) {
			examine_item(d->PC->equipped[gloves]);
		  }
		  break;
		case 'h':
		  if (d->PC->equipped[boots] != NULL) {
			examine_item(d->PC->equipped[boots]);
		  }
		  break;
		case 'i':
		  if (d->PC->equipped[amulet] != NULL) {
			examine_item(d->PC->equipped[amulet]);
		  }
		  break;
		case 'j':
		  if (d->PC->equipped[light] != NULL) {
			examine_item(d->PC->equipped[light]);
		  }
		  break;
		case 'k':
		  if (d->PC->equipped[ring_one] != NULL) {
			examine_item(d->PC->equipped[ring_one]);
		  }
		  break;
		case '1':
		  if (d->PC->equipped[ring_two] != NULL) {
			examine_item(d->PC->equipped[ring_two]);
		  }
		  break;
		default: break;
	  }
	  break;
	default: break;
  }
  clear_screen();
}
void examine_item(object *o) {
  clear_screen();
  unsigned x = 10, y = 1, counter = 0, desc_index = 0;
  while (desc_index < strlen(o->get_desc())) {
	mvaddch(y, x, o->get_desc()[desc_index]);
	++desc_index;
	++x;
	++counter;
	if (counter > 39) {
	  counter = 0;
	  ++y;
	  x = 10;
	}
  }
  getch();
}

void examine_monster(const char* description) {
  clear_screen();
  unsigned x = 10, y = 1, counter = 0, desc_index = 0;
  while(desc_index < strlen(description)) {
    mvaddch(y, x, description[desc_index]);
    ++desc_index;
    ++x;
    ++counter;
    if(counter > 39) {
      counter = 0;
      ++y;
      x = 10;
    }
  }
  getch();
}

void io_examine_monster(dungeon_t *d) { 
  pair_t dest;
  int c;
  fd_set readfs;
  struct timeval tv;

  io_display_no_fog(d);

  mvprintw(0, 0, "Select a monster with. 't' to inspect; Press escape to abort");

  dest[dim_y] = d->PC->position[dim_y];
  dest[dim_x] = d->PC->position[dim_x];

  mvaddch(dest[dim_y] + 1, dest[dim_x], '*');
  refresh();

  do {
	do {
	  FD_ZERO(&readfs);
	  FD_SET(STDIN_FILENO, &readfs);

	  tv.tv_sec = 0;
	  tv.tv_usec = 125000; /* An eigth of a second */

	  io_redisplay_non_terrain(d, dest);
	} while (!select(STDIN_FILENO + 1, &readfs, NULL, NULL, &tv));
	/* Can simply draw the terrain when we move the cursor away, *
	 * because if it is a character or object, the refresh       *
	 * function will fix it for us.                              */
	switch (mappair(dest)) {
	  case ter_wall:
	  case ter_wall_immutable:
	  case ter_unknown:mvaddch(dest[dim_y] + 1, dest[dim_x], ' ');
		break;
	  case ter_floor:
	  case ter_floor_room:mvaddch(dest[dim_y] + 1, dest[dim_x], '.');
		break;
	  case ter_floor_hall:mvaddch(dest[dim_y] + 1, dest[dim_x], '#');
		break;
	  case ter_debug:mvaddch(dest[dim_y] + 1, dest[dim_x], '*');
		break;
	  case ter_stairs_up:mvaddch(dest[dim_y] + 1, dest[dim_x], '<');
		break;
	  case ter_stairs_down:mvaddch(dest[dim_y] + 1, dest[dim_x], '>');
		break;
	  default:
		/* Use zero as an error symbol, since it stands out somewhat, and it's *
		 * not otherwise used.                                                 */
		mvaddch(dest[dim_y] + 1, dest[dim_x], '0');
	}
	switch ((c = getch())) {
	  case '7':
	  case 'y':
	  case KEY_HOME:
		if (dest[dim_y] != 1) {
		  dest[dim_y]--;
		}
		if (dest[dim_x] != 1) {
		  dest[dim_x]--;
		}
		break;
	  case '8':
	  case 'k':
	  case KEY_UP:
		if (dest[dim_y] != 1) {
		  dest[dim_y]--;
		}
		break;
	  case '9':
	  case 'u':
	  case KEY_PPAGE:
		if (dest[dim_y] != 1) {
		  dest[dim_y]--;
		}
		if (dest[dim_x] != DUNGEON_X - 2) {
		  dest[dim_x]++;
		}
		break;
	  case '6':
	  case 'l':
	  case KEY_RIGHT:
		if (dest[dim_x] != DUNGEON_X - 2) {
		  dest[dim_x]++;
		}
		break;
	  case '3':
	  case 'n':
	  case KEY_NPAGE:
		if (dest[dim_y] != DUNGEON_Y - 2) {
		  dest[dim_y]++;
		}
		if (dest[dim_x] != DUNGEON_X - 2) {
		  dest[dim_x]++;
		}
		break;
	  case '2':
	  case 'j':
	  case KEY_DOWN:
		if (dest[dim_y] != DUNGEON_Y - 2) {
		  dest[dim_y]++;
		}
		break;
	  case '1':
	  case 'b':
	  case KEY_END:
		if (dest[dim_y] != DUNGEON_Y - 2) {
		  dest[dim_y]++;
		}
		if (dest[dim_x] != 1) {
		  dest[dim_x]--;
		}
		break;
	  case '4':
	  case 'h':
	  case KEY_LEFT:
		if (dest[dim_x] != 1) {
		  dest[dim_x]--;
		}
		break;
	}
  } while (c != 't' && c != 27);

  if (charpair(dest) && charpair(dest) != d->PC) {
    const char* monster_to_examine = get_npc_desc(dynamic_cast<npc *>(d->character_map[dest[dim_y]][dest[dim_x]]));
    examine_monster(monster_to_examine);
  }

  io_display(d);
}

void io_handle_input(dungeon *d) {
  uint32_t fail_code;
  int key;
  fd_set readfs;
  struct timeval tv;
  uint32_t fog_off = 0;
  pair_t tmp = {DUNGEON_X, DUNGEON_Y};

  do {
	do {
	  FD_ZERO(&readfs);
	  FD_SET(STDIN_FILENO, &readfs);

	  tv.tv_sec = 0;
	  tv.tv_usec = 125000; /* An eigth of a second */

	  if (fog_off) {
		/* Out-of-bounds cursor will not be rendered. */
		io_redisplay_non_terrain(d, tmp);
	  } else {
		io_redisplay_visible_monsters(d);
	  }
	} while (!select(STDIN_FILENO + 1, &readfs, NULL, NULL, &tv));
	fog_off = 0;
	switch (key = getch()) {
	  case '7':
	  case 'y':
	  case KEY_HOME:fail_code = move_pc(d, 7);
		break;
	  case '8':
	  case 'k':
	  case KEY_UP:fail_code = move_pc(d, 8);
		break;
	  case '9':
	  case 'u':
	  case KEY_PPAGE:fail_code = move_pc(d, 9);
		break;
	  case '6':
	  case 'l':
	  case KEY_RIGHT:fail_code = move_pc(d, 6);
		break;
	  case '3':
	  case 'n':
	  case KEY_NPAGE:fail_code = move_pc(d, 3);
		break;
	  case '2':
	  case 'j':
	  case KEY_DOWN:fail_code = move_pc(d, 2);
		break;
	  case '1':
	  case 'b':
	  case KEY_END:fail_code = move_pc(d, 1);
		break;
	  case '4':
	  case 'h':
	  case KEY_LEFT:fail_code = move_pc(d, 4);
		break;
	  case '5':
	  case ' ':
	  case '.':
	  case KEY_B2:fail_code = 0;
		break;
	  case '>':fail_code = move_pc(d, '>');
		break;
	  case '<':fail_code = move_pc(d, '<');
		break;
	  case 'Q':d->quit = 1;
		fail_code = 0;
		break;
	  case 'T':
		/* New command.  Display the distances for tunnelers.             */
		io_display_tunnel(d);
		fail_code = 1;
		break;
	  case 'D':
		/* New command.  Display the distances for non-tunnelers.         */
		io_display_distance(d);
		fail_code = 1;
		break;
	  case 'H':
		/* New command.  Display the hardnesses.                          */
		io_display_hardness(d);
		fail_code = 1;
		break;
	  case 's':
		/* New command.  Return to normal display after displaying some   *
		 * special screen.                                                */
		io_display(d);
		fail_code = 1;
		break;
	  case 'g':
		/* Teleport the PC to a random place in the dungeon.              */
		io_teleport_pc(d);
		fail_code = 1;
		break;
	  case 'm':io_list_monsters(d);
		fail_code = 1;
		break;
	  case 'f':io_display_no_fog(d);
		fog_off = 1;
		fail_code = 1;
		break;
	  case 'i':
		io_display_inv(d);
		fail_code = 1;
		break;
	  case 'e':
		io_display_eq(d);
		fail_code = 1;
		break;
	  case 'L': 
		io_examine_monster(d);
		fail_code = 1;
		break;
	  case 'q':
		/* Demonstrate use of the message queue.  You can use this for *
		 * figure out why I did it that way.                           */
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
		fail_code = 0;
		break;
	  default:
		/* Also not in the spec.  It's not always easy to figure out what *
		 * key code corresponds with a given keystroke.  Print out any    *
		 * unhandled key here.  Not only does it give a visual error      *
		 * indicator, but it also gives an integer value that can be used *
		 * octal, thus allowing us to do reverse lookups.  If a key has a *
		 * name defined in the header, you can use the name here, else    *
		 * you can directly use the octal value.                          */
		mvprintw(0, 0, "Unbound key: %#o ", key);
		fail_code = 1;
	}
  } while (fail_code);
}
