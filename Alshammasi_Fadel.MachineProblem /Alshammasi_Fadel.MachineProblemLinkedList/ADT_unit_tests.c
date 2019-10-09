#include <stdio.h>
#include <stdlib.h>
/* including the header of the thing we want to test */
#include "coms326x_linkedlist.h"


/* a full unit test for all linkedlist functionality */
int main(){
   
  printf("LinkedList test\n \n");

  LIST_T mylist;

  init_list(&mylist); /* initilaize the list */
  printf("There's no capacity concept in linked list \n");
  printf("The number of bytes allocated for size of 0 is: %d\n", mylist.allocated(&mylist)); /* should return 0 as 0(size) * 4 =0 */


  /* test of size function + is Empty */
  printf("Size of the list is: %d \n", mylist.getSize(&mylist)); /* size should be 0 since list is empty */
  printf("Is the list Empty? 1 for yes and 0 for no: %d \n", mylist.isEmpty(&mylist)); /* should return 1 because no element has been added yet */
 

  /* test of the adding functions (line 27 tests inserting when an item is already there so everthing following is advancedin position + test the element function in the loop */
  mylist.insert(&mylist,0,10); /* list: 10 */
  mylist.insert(&mylist,1,20); /* list: 10 -> 20 */
  mylist.append(&mylist,30); /* add the end since list already has elements: list 0-> 10 -> 20 -> 30 */
  mylist.prepend(&mylist,40);  /* add at the begining when list already has elements list: 40 -> 10 -> 20 -> 30 */
  mylist.insert(&mylist,1,50); /* list: 40 -> 50 -> 10 -> 20 -> 30 */

  
  int i;
  int size= mylist.getSize(&mylist); 
  
  /* printing the list: should be: 40,50,10,20,30 */
  printf("The list is: ");
  for( i=0 ; i< size; i++){
    if (i== size-1){
      printf("%d ", mylist.element(&mylist,i)); /* if it's the last element don't put a comma after it */

    }
    else {
      printf("%d ,", mylist.element(&mylist,i)); /* place a comma after printing the number */

    }

  }
  printf("\n");


  /* check allocated bytes now that there are 5 ints in the list */
  printf("There's no capacity concept in linked list \n");
  printf("The number of bytes allocated for size of 5 is: %d\n", mylist.allocated(&mylist)); /* should return 20 as 5(size) * 4 =20 */



/* check if empty changes after elements added */
  printf("Is the list Empty? 1 for yes and 0 for no: %d \n", mylist.isEmpty(&mylist)); /* should return 0 because elements have been added */

  /* test for the head function */
  printf("The head of the linkedlist is: %d \n", mylist.head(&mylist)); /* should return 40 since it's the first element */


  /* test tail function */
    int *temp=mylist.tail(&mylist); /* storing the array in a varible */
     size= mylist.getSize(&mylist)-1; /* subrtact one since we are not counting the head */
      printf("The Elements in the linkedlist except the first one are(testing the tail function) : ");
      /* should return: 50,10,20,30 */
       for(i=0; i<size; i++){
          if(i==size-1){
	 	 printf("%d ", temp[i]);
	   } else{
	       printf("%d,", temp[i]);
	         }
          }

	  printf("\n"); 
	  free(temp); /* free temp after useage */
	  
	  /* test delete function in different positions */
	  /* list before deleting anything : 40, 50, 10, 20, 30 */ 
	  mylist.delete(&mylist,0); /* delete at the begining: list: 50, 10, 20, 30 */
	  mylist.delete(&mylist,1); /* delete in the middle: list: 50, 20, 30 */
	  mylist.delete(&mylist,2); /* delete at the end: list : 50, 20 */

	  size= mylist.getSize(&mylist); /* update the varible with the current size to use in loop */

  /* printing the list: should be: 50, 20 */
  printf("The list after deleting 40(begining), 10(middle), and 30(end) is: ");
  for( i=0 ; i< size; i++){
    if (i== size-1){
      printf("%d ", mylist.element(&mylist,i)); /* if it's the last element don't put a comma after it */

    }
    else {
      printf("%d ,", mylist.element(&mylist,i)); /* place a comma after printing the number */

    }

  }
  printf("\n");
  /* check if size updates after deletion */
  printf("Size of list after deleting 40,10, and 30 is : %d\n ", mylist.getSize(&mylist)); /* should return 2 */
  

  /* test of prepend when list is empty */
  /* deleting everthing */
  mylist.delete(&mylist,0); /* delete 50: list: 20 */
  mylist.delete(&mylist,0); /* delete 20: list: empty */

  /* adding to the begining when list is empty */
  mylist.prepend(&mylist,100);
  
  printf("Testing the case when prepending 100 to an empty list: ");
  /* list should be: 100 */
  for(i=0; i<mylist.getSize(&mylist); i++){
    if(i==mylist.getSize(&mylist)-1){
        printf("%d ", mylist.element(&mylist,i)); 

    }
    else {
      printf("%d ,", mylist.element(&mylist,i)); 

    }

  }
  printf("\n");

  /* test of append when test is empty*/
  mylist.delete(&mylist,0);  /* let the list empty again to test append to an empty list.this also test deleting when there's only single element in the list */

  mylist.append(&mylist,200);   /* adding to the end when list is empty */

 printf("Testing the case when appending 200 to an empty list: ");
  /* list should be: 200 */
  for(i=0; i<mylist.getSize(&mylist); i++){
    if(i==mylist.getSize(&mylist)-1){
        printf("%d ", mylist.element(&mylist,i)); 

    }
    else {
      printf("%d ,", mylist.element(&mylist,i)); 

    }

  }
  printf("\n");

  mylist.delete(&mylist,0); /* delete everthing to free memory (in fact it's the only element there) */
  printf("There's no capacity concept in linked list \n");
  printf("The number of bytes allocated for the empty linked list: %d\n", mylist.allocated(&mylist)); /* should be 0 because 0*4(size of int in my machine) =0 */
  printf("All functions have been tested successfuly!!\n "); /* Done testing everthing */


  return 0; /* if no error */

}
