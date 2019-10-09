#ifndef DIMS_H
# define DIMS_H
typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef enum equipment {
  weapon,
  offhand,
  ranged,
  armor,
  helmet,
  cloak,
  gloves,
  boots,
  amulet,
  light,
  ring_one,
  ring_two
} equipment_t;

typedef int16_t pair_t[num_dims];

#endif
