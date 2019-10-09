#ifndef COMS326X_STACK_H_INCLUDED /* if it's not included, then include it */

#define COMS326X_STACK_H_INCLUDED





#define EMPTY_STACK -1 /* negative value to denote that stack is empty */

#define STACK_SUCCESS 1

#define STACK_FAILURE 0



typedef struct stackItem STACK_ITEM_T; /* making an alias for stackItem */

typedef struct stack STACK_T; /* making an alias for "stack" */


/* a structure that contains basic stack requirements */
struct stack {

  STACK_ITEM_T *head ; /* pointer to the head of the stack */

  int size; /* size variable */

  int (*pop)(STACK_T *); /* a pointer to the  pop function */

  void (*push)(STACK_T *, int); /* a pointer to the push function */ 

  int (*peek)(STACK_T *); /* a pointer to the peek function */


  int (*getSize)(STACK_T *); /* a pointer to the size function */

  
};


/* another structure for the stack's items */
struct stackItem {

  int value; /* the value store in a particular item */

  STACK_ITEM_T *next; /* a pointer to the next item of a particular item */

};



void init_stack(STACK_T *self); /* a prototype to a function to initialize the stack and its functionality */




int __stack_pop(STACK_T *self); /* a prototype for  Pop which returns the value of the top of the stack with removing it */




void __stack_push(STACK_T *self, int item); /* a prototype push adds a new value to the stack without returning it */


int __stack_peek(STACK_T *self); /* a prototype to Peek which returns the value of the top of the stack without changing the stack itself */



int __stack_size(STACK_T *self); /* a prototype to  size which returns the size of the stack */






#endif /* COMS326X_STACK_H_INCLUDED */

