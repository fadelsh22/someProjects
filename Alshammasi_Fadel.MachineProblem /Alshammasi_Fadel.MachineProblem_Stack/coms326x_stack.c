#include <stdlib.h>

#include <stddef.h>

/* including the header file of the class which contains the prototypes of the functions and the needed structures */
#include "coms326x_stack.h"



/* a function to initialize the stack and its functionality */
void init_stack(STACK_T *stack) {

  stack->head = NULL; /* an empty stack doesn't have values yet so we set it to null*/

  stack->size = 0; /* if nothing is then it means size is 0 */

  stack->pop = &__stack_pop; /* initalizing the pop function */

  stack->push = &__stack_push; /* initalizing the push function */
  
  stack->peek=&__stack_peek; /* initalizing the peek function */

  stack->getSize = &__stack_size; /* initalizing the size function */

}


/* Pop returns the value of the top of the stack with removing it */
int __stack_pop(STACK_T *self) {

  /* if the top is null it means the stack is empty so we denote it by a negative numebr (-1) */
  if (self->head == NULL) { 

    return EMPTY_STACK; 

  }



  STACK_ITEM_T *top = self->head; /* variable to access the store the top of the stack*/

  int retVal = top->value; /* accessing to top's value and storing it in retVal */



  self->head = top->next; /* removing the top and assign the new head to the next element */

  free(top); /* we free what we just got rid of because it's not in the stack */

  self->size--; /* size decrements by one after removing the old top */

  return(retVal); /* returning the value of the old top that we removed */

}


/* push adds a new value to the stack without returning it */
void __stack_push(STACK_T *self, int item) {

  STACK_ITEM_T *new_item = (STACK_ITEM_T *)malloc(1*sizeof(STACK_ITEM_T)); /* allocating memory for the new element to be added */



  new_item->value = item; /* assigning the new element's to the desired value entered by user */

  new_item->next = self->head; /* putting the new item on top of stack */

  self->head = new_item; /* top of the stack is now the item we added */

  self->size++; /* adding an item increments the size by 1 */



}

/* Peek returns the value of the top of the stack without changing the stack itself */
int __stack_peek(STACK_T *self) {

  /* if the top is null it means the stack is empty so we denote it by a negative numebr (-1) */
  if(self->head==NULL){
    return EMPTY_STACK;
  }
  
  STACK_ITEM_T *top=self->head; /* variable to access the store the top of the stack*/
  int value=top->value; /* accessing to top's value and storing it in value */

  return (value); /* returning the varible that contains the top of the stack without removing it */


}



/* size returns the size of the stack */
int __stack_size(STACK_T *self) {

  return(self->size); /* return the current size */

}

