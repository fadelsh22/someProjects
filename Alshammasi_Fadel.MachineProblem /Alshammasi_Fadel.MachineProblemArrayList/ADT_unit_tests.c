#include <stdio.h>
#include <stdlib.h>
/* including the header of the thing we want to test */
#include "coms326x_arraylist.h"


/* a full unit test for all arraylist functionality */
int main(){
   
  printf("ArrayList test\n \n");

  LIST_T mylist;

  init_list(&mylist); /* initilaize the list */
  
  /*test for initial capacity + and number of bytes allocated for it */

  printf("The capacity of this empty list is: %d\n",mylist.capacity); /* should be 5 as I set up the constant to be initially like that */
  printf("The number of bytes allocated for size of 0 and a capacity of 5 is: %d\n", mylist.allocated(&mylist)); /* should be 20 as capacity (initial) is 5 and size of int in my machine is 4  so 5*4=20 */


   /* test of size function + is Empty */
  printf("Size of the list is: %d \n", mylist.getSize(&mylist)); /* size should be 0 since list is empty */
  printf("Is the list Empty? 1 for yes and 0 for no: %d \n", mylist.isEmpty(&mylist)); /* should return 1 because no element has been added yet */

   /* test of the adding functions (line 25 tests inserting when an item is already there so everthing following is advanced in position + test the element function in the loop */
  mylist.insert(&mylist,0,10); /* list: 10 */
  mylist.insert(&mylist,1,20); /* list: 10 , 20 */
  mylist.append(&mylist,30); /* add the end since list already has elements: list 0, 10 , 20 , 30 */
  mylist.prepend(&mylist,40);  /* add at the begining when list already has elements list: 40 , 10 , 20 , 30 */
  mylist.insert(&mylist,1,50); /* list: 40 , 50 , 10 , 20 , 30 */

  
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
  
  /*Test if capacity and bytes allocated stay the same after adding 5 elements(initial capacity)*/
   printf("The capacity of this list of 5 ints is: %d\n",mylist.capacity); /* should still be 5 as size hasn't gone over 5 yet */
  printf("The number of bytes allocated for size of 5 and a capacity of 5 is: %d\n", mylist.allocated(&mylist)); /* should still be 20 as capacity is same */

  /* let's add a 6th element to ensure that growth of capacity and therefore bytes allocated */
  mylist.append(&mylist,300);
  printf("The capacity of this list of 6 is: %d\n",mylist.capacity); /* should be 10 (no more than twice as spec) because size went over 5 */
  printf("The number of bytes allocated for size of 6 and a capacity of 10 is: %d\n", mylist.allocated(&mylist)); /* should be 40 as capacity doubled to 10 and int in my machine is 4 bytes each */

  mylist.delete(&mylist,5);

/* check if empty changes after elements added */
  printf("Is the list Empty? 1 for yes and 0 for no: %d \n", mylist.isEmpty(&mylist)); /* should return 0 because elements have been added */

  /* test for the head function */
  printf("The head of the list is: %d \n", mylist.head(&mylist)); /* should return 40 since it's the first element */


  /* test tail function */
    int *temp=mylist.tail(&mylist); /* storing the array in a varible */
     size= mylist.getSize(&mylist)-1; /* subrtact one since we are not counting the head */
      printf("The Elements in the arraylist except the first one are(testing the tail function) : ");
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

  /* test that capacity (by a factor of 2) and therefore bytes allocated shrink */
  mylist.delete(&mylist,0);
  printf("The capacity after deleting everthing: %d\n", mylist.capacity); /* should be 1 */
  printf("The number of bytes allocated for the arraylist: %d\n", mylist.allocated(&mylist)); /* 1*4=4 */

  free(mylist.arr); /* free memory */

  printf("All the functions of array list have been successfuly tested!!\n");
  
  return 0; /* if no error */


}
