#include <stdio.h>

#include <stdlib.h>

/* including the header of the thing we want to test */
#include "coms326x_stack.h"


/* a full unit test for all stack functionality */
int main()

{

    printf("Stack test\n"); 





    STACK_T mystack; /* creating a stack variable */



    init_stack(&mystack); /* initializing the stack with our stack's address */

    printf("%d\n", mystack.peek(&mystack)); /* stack is empty it should return -1 */


    mystack.push(&mystack, 10); /* pushing an item with the a value of 10 into the stack */
    printf("%d\n", mystack.getSize(&mystack)); /* should return 1 because only 10 is there */
    printf("%d\n", mystack.peek(&mystack)); /* should return 10 because it's the element on top(only element in fact so far) */


      mystack.push(&mystack, 20); /* pushing an item with the a value of 20 into the stack */
      printf("%d\n", mystack.getSize(&mystack)); /* should return 2 */
      printf("%d\n", mystack.peek(&mystack)); /* should return 20 because it's on top */


      mystack.push(&mystack, 30); /* pushing an item with the a value of 30 into the stack */
      printf("%d\n", mystack.getSize(&mystack)); /* should return 3 */
       printf("%d\n", mystack.peek(&mystack)); /* should return 30 because it's on top */


      mystack.push(&mystack, 40); /* pushing an item with the a value of 40 into the stack */
      printf("%d\n", mystack.getSize(&mystack)); /* should return 4 */
      printf("%d\n", mystack.peek(&mystack)); /* should return 40 because it's on top */


      mystack.push(&mystack, 50); /* pushing an item with the a value of 50 into the stack */
      printf("%d\n", mystack.getSize(&mystack)); /* should return 5 */
      printf("%d\n", mystack.peek(&mystack)); /* should return 50 because it's on top */


      while(mystack.getSize(&mystack) > 0) { /* continue as long as the stack is not empty */

        printf("POP %d\n", mystack.pop(&mystack)); /* pop the element on top one by one and print (so 50,40,30,20,10) until stack is empty */

      }

      printf("%d\n", mystack.peek(&mystack)); /* everthing has been popped so stack should be empty so it should return -1 */


      return 0; /* if no error */

      /* there's no memory leak because every item that we pushed (therefore allocated some memory), we called the pop function which has the "free" function to deallocate */

}

