#ifndef IO_H
# define IO_H
#include "object.h"
#define NUM_INVENTORY              10
typedef struct dungeon dungeon_t;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon_t *d);
void io_handle_input(dungeon_t *d);
void io_queue_message(const char *format, ...);
void wear_equipment(dungeon_t *d, int invent_slot);
void io_display_inv(dungeon_t *d);
void io_display_eq(dungeon_t *d);
void examine_item(object *o);
void io_examine_monster(dungeon_t *d);
#endif
