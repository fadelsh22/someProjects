#include "move.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"
#include "event.h"
#include "io.h"
#include "npc.h"

int is_boss_alive = 1;

void do_combat(dungeon *d, character *atk, character *def) {
  const char *organs[] = {
	  "liver",                   /*  0 */
	  "pancreas",                /*  1 */
	  "heart",                   /*  2 */
	  "eye",                     /*  3 */
	  "arm",                     /*  4 */
	  "leg",                     /*  5 */
	  "intestines",              /*  6 */
	  "gall bladder",            /*  7 */
	  "lungs",                   /*  8 */
	  "hand",                    /*  9 */
	  "foot",                    /* 10 */
	  "spinal cord",             /* 11 */
	  "pituitary gland",         /* 12 */
	  "thyroid",                 /* 13 */
	  "tongue",                  /* 14 */
	  "bladder",                 /* 15 */
	  "diaphram",                /* 16 */
	  "stomach",                 /* 17 */
	  "pharynx",                 /* 18 */
	  "esophagus",               /* 19 */
	  "trachea",                 /* 20 */
	  "urethra",                 /* 21 */
	  "spleen",                  /* 22 */
	  "ganglia",                 /* 23 */
	  "ear",                     /* 24 */
	  "subcutaneous tissue"      /* 25 */
	  "cerebellum",              /* 26 */ /* Brain parts begin here */
	  "hippocampus",             /* 27 */
	  "frontal lobe",            /* 28 */
	  "brain",                   /* 29 */
  };

  if (atk != d->PC && def != d->PC) { 
	character *temp = def;
	int atk_y = character_get_y(atk);
	int atk_x = character_get_x(atk);
	d->character_map[character_get_y(def)][character_get_x(def)] = atk;
	d->character_map[atk_y][atk_x] = temp;
	return;
  }
  if (atk == d->PC && character_is_alive(def)) {
	uint32_t atk_dmg = d->PC->damage->roll(); 
	for (int i = 0; i < NUM_EQUIPMENT; ++i) { 
	  if (d->PC->equipped[i]) {
		atk_dmg += d->PC->equipped[i]->roll_dice();
	  }
	}

	io_queue_message("You deal %d damage to %s", atk_dmg, character_get_name(def));
	def->hp -= atk_dmg;
	if (def->hp < 0) { 
	  --d->num_monsters;
	  character_die(def);
	  character_increment_dkills(atk);
	  character_increment_ikills(atk, (character_get_dkills(def)) +
		  character_get_ikills(def));
	  io_queue_message("You killed %s %s", character_get_name(def), organs[1]);
	  if (def->sequence_number == NPC_BOSS) {
		is_boss_alive = 0;
	  }
	}
  }

  /*NPC attacking PC*/
  if(atk != d->PC && character_is_alive(def) && def == d->PC) {
    int32_t atk_dmg = atk->damage->roll();
    if(def->hp < atk_dmg) { 
      character_die(def);
      character_increment_dkills(atk);
      character_increment_ikills(atk, (character_get_dkills(def)) +
	      character_get_ikills(def));
      io_queue_message("Your last thoughts fade away as "
                       "%s%s eats your %s...",
                       is_unique(atk) ? "" : "the ",
                       atk->name, organs[rand_range(0, 29)]);
      io_queue_message("");
    }
    else { 
      def->hp -= atk_dmg;
    }
  }
}

int boss_alive() {
  return is_boss_alive;
}

void move_character(dungeon *d, character *c, pair_t next) {
  if (charpair(next) &&
	  ((next[dim_y] != c->position[dim_y]) ||
		  (next[dim_x] != c->position[dim_x]))) {
	do_combat(d, c, charpair(next));
  } else {
	/* No character in new position. */

	d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
	c->position[dim_y] = next[dim_y];
	c->position[dim_x] = next[dim_x];
	d->character_map[c->position[dim_y]][c->position[dim_x]] = c;
  }

  if (c == d->PC) {
	if (d->objmap[next[dim_y]][next[dim_x]]) { 
	  for (int i = 0; i < NUM_INVENTORY; ++i) {
		if (d->PC->inventory[i] == NULL) { 
		  d->PC->inventory[i] = d->objmap[next[dim_y]][next[dim_x]];
		  d->objmap[next[dim_y]][next[dim_x]] = NULL; 
		}
	  }
	}
	pc_reset_visibility(d->PC);
	pc_observe_terrain(d->PC, d);
  }
}

void do_moves(dungeon *d) {
  pair_t next;
  character *c;
  event_t *e;

  if (pc_is_alive(d)) {
	/* The PC always goes first one a tie, so we don't use new_event().  *
	 * We generate one manually so that we can set the PC sequence       *
	 * number to zero.                                                   */
	e = (event_t *) malloc(sizeof(*e));
	e->type = event_character_turn;
	/* Hack: New dungeons are marked.  Unmark and ensure PC goes at d->time, *
	 * otherwise, monsters get a turn before the PC.                         */
	if (d->is_new) {
	  d->is_new = 0;
	  e->time = d->time;
	} else {
	  e->time = d->time + (1000 / d->PC->speed);
	}
	e->sequence = 0;
	e->c = d->PC;
	heap_insert(&d->events, e);
  }

  while (pc_is_alive(d) &&
	  (e = (event_t *) heap_remove_min(&d->events)) &&
	  ((e->type != event_character_turn) || (e->c != d->PC))) {
	d->time = e->time;
	if (e->type == event_character_turn) {
	  c = e->c;
	}
	if (!c->alive) {
	  if (d->character_map[c->position[dim_y]][c->position[dim_x]] == c) {
		d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
	  }
	  if (c != d->PC) {
		event_delete(e);
	  }
	  continue;
	}

	npc_next_pos(d, (npc *) c, next);
	move_character(d, c, next);

	heap_insert(&d->events, update_event(d, e, 1000 / c->speed));
  }

  io_display(d);
  if (pc_is_alive(d) && e->c == d->PC) {
	c = e->c;
	d->time = e->time;
	e->c = NULL;
	event_delete(e);
	io_handle_input(d);
  }
}

void dir_nearest_wall(dungeon *d, character *c, pair_t dir) {
  dir[dim_x] = dir[dim_y] = 0;

  if (c->position[dim_x] != 1 && c->position[dim_x] != DUNGEON_X - 2) {
	dir[dim_x] = (c->position[dim_x] > DUNGEON_X - c->position[dim_x] ? 1 : -1);
  }
  if (c->position[dim_y] != 1 && c->position[dim_y] != DUNGEON_Y - 2) {
	dir[dim_y] = (c->position[dim_y] > DUNGEON_Y - c->position[dim_y] ? 1 : -1);
  }
}

uint32_t against_wall(dungeon *d, character *c) {
  return ((mapxy(c->position[dim_x] - 1,
                 c->position[dim_y]) == ter_wall_immutable) ||
	  (mapxy(c->position[dim_x] + 1,
	         c->position[dim_y]) == ter_wall_immutable) ||
	  (mapxy(c->position[dim_x],
	         c->position[dim_y] - 1) == ter_wall_immutable) ||
	  (mapxy(c->position[dim_x],
	         c->position[dim_y] + 1) == ter_wall_immutable));
}

uint32_t in_corner(dungeon *d, character *c) {
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(c->position[dim_x] - 1,
                          c->position[dim_y]) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x] + 1,
                          c->position[dim_y]) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x],
                          c->position[dim_y] - 1) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x],
                          c->position[dim_y] + 1) == ter_wall_immutable);

  return num_immutable > 1;
}

static void new_dungeon_level(dungeon *d, uint32_t dir) {

  switch (dir) {
	case '<':
	case '>':new_dungeon(d);
	  break;
	default:break;
  }
}

uint32_t move_pc(dungeon *d, uint32_t dir) {
  pair_t next;
  uint32_t was_stairs = 0;

  next[dim_y] = d->PC->position[dim_y];
  next[dim_x] = d->PC->position[dim_x];

  switch (dir) {
	case 1:
	case 2:
	case 3:next[dim_y]++;
	  break;
	case 4:
	case 5:
	case 6:break;
	case 7:
	case 8:
	case 9:next[dim_y]--;
	  break;
  }
  switch (dir) {
	case 1:
	case 4:
	case 7:next[dim_x]--;
	  break;
	case 2:
	case 5:
	case 8:break;
	case 3:
	case 6:
	case 9:next[dim_x]++;
	  break;
	case '<':
	  if (mappair(d->PC->position) == ter_stairs_up) {
		was_stairs = 1;
		new_dungeon_level(d, '<');
	  }
	  break;
	case '>':
	  if (mappair(d->PC->position) == ter_stairs_down) {
		was_stairs = 1;
		new_dungeon_level(d, '>');
	  }
	  break;
  }

  if (was_stairs) {
	return 0;
  }

  if ((dir != '>') && (dir != '<') && (mappair(next) >= ter_floor)) {
	move_character(d, d->PC, next);
	dijkstra(d);
	dijkstra_tunnel(d);

	return 0;
  }
  return 1;
}
