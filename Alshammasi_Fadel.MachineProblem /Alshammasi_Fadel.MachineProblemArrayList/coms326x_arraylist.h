#ifndef COMS326X_ARRAYLIST_H_INCLUDED
#define COMS326X_ARRAYLIST_H_INCLUDED

/* constants for success and error */
#define LIST_SUCCESS 1
#define LIST_ERROR 0

/* initial capacity is set to 5 by me */
#define INITIAL_CAPACITY 5

typedef struct list LIST_T; /* making an alias for LIST_ITEM */

 /* a structure that contains basic arraylist requirements */
struct list{

int size, capacity;
  int *arr; /* the int array that we're using */

  /* pointers to functions */
  int (*getSize)(LIST_T *);
  int (*isEmpty)(LIST_T *);
  int (*element)(LIST_T *, int);
  int (*head)(LIST_T *);
  int *(*tail)(LIST_T *);
  int (*insert)(LIST_T *,  int, int);
  void (*prepend)(LIST_T *, int);
  void (*append)(LIST_T *, int);
  int (*delete)(LIST_T *, int);
  int (*allocated)(LIST_T *);

};

/* functions prototypes */
int init_list(LIST_T *theList);
int __list_size(LIST_T *self);
int __list_isEmpty(LIST_T *self);
int __list_element(LIST_T *self, int index);
int __list_head(LIST_T *self);
int *__list_tail(LIST_T *self);
int __list_insert(LIST_T *self, int index, int value);
void __list_prepend(LIST_T *self, int value);
void __list_append(LIST_T *self, int value);
int __list_delete(LIST_T *self, int index);
int __list_allocated(LIST_T *self);

#endif
