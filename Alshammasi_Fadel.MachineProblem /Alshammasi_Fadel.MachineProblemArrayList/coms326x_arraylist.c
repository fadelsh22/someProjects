#include <stdlib.h>

#include <stddef.h>
/* including the header file of the class which contains the prototypes of the functions and the needed structures */
#include "coms326x_arraylist.h"

/* Initializes a new list, and returns LIST_SUCess on  success, LIST_ERROR otherwise */
int init_list(LIST_T *theList){

  theList->size=0; /* size is initially set to 0 as list is empty */
  theList->capacity=INITIAL_CAPACITY; /* initialize capacity to its constant (5) */
  theList->arr=malloc (theList->capacity * sizeof(int)); /* dynamiccally allocate memory to our array */

/* initializing the pointers to the functions to their functions */
theList->getSize=&__list_size;
theList->isEmpty=&__list_isEmpty;
theList->element=&__list_element;
theList->head=&__list_head;
theList->tail=&__list_tail;

theList->insert=&__list_insert;
theList->prepend=&__list_prepend;
theList->append=&__list_append;
theList->delete=&__list_delete;
theList->allocated=&__list_allocated; 

 return LIST_SUCCESS; /* if no error */

}

/* Returns the current size of the list */
int __list_size(LIST_T *self){
 return(self->size);

}

/* Return 1 if the list is empty, 0 otherwise */
int __list_isEmpty(LIST_T *self){

  if(__list_size(self)==0){ /* if size is 0 then it's empty therefore return 1 */
     return 1;
   }
  return 0; /* 0 otherwise */

} 

/* Returns the list value at index, if it exists and a constant LIST_ERROR otherwixe */
int __list_element(LIST_T *self, int index){
 
  if( index < 0 || index >= self->size){  /* checking bounds*/
    return LIST_ERROR;
     }

 return (self->arr[index]);
}

/* Returns the head of the list, if it exists, and LIST_ERROR otherwise */
int __list_head(LIST_T *self){
  
  return (__list_element(self,0)); /*head is always the the element at index 0 so we call element with 0 as index */

 }

/* Returns an array of integers containing the value of all components of the list except the fitst in a newly allocated int array */
int *__list_tail(LIST_T *self){

  int size=(__list_size(self)-1); /* subtract one since head is not counted */
  int *temp=(int *)malloc(size*sizeof(int)); /* newly allocated array to store the tail */
   
   int i,j;
   /* store everthing from our array in temp except the head */
   for(i=0, j=1; i<size;i++,j++){
     temp[i]=self-> arr[j];
   }

  return temp;

}

/* Adds a new node with value to the list at index, if index <= the list's size, if an item already exists at that index, that item and all following it are advanced in position. Returns LIST_SUCCESS if insertion was successful; LIST_ERROR otherwise */
int __list_insert(LIST_T *self, int index, int value){
  if(index<0 || index > self->size ){ /* bound checking */
    return LIST_ERROR;
  }

  if(self->size >= self->capacity){ /* if there's no enough capacity,resize */
    self->capacity*=2; /* double capacity (should not be greater than 2 as spec */
    self->arr=realloc(self->arr, self->capacity *sizeof(int)); /* resize the array with the new capacity */
  }

  int i;
  /* adding the element */
  for(i= self->size ; i>index; i--){
    self->arr[i]=self->arr[i-1];
  }

  self->arr[index]=value; /* providing the entered value in that position */
  self->size++; /* increments size */
  
  return LIST_SUCCESS; /* if no error */

}

/* Adds a new node with value to the begining of the list */
void __list_prepend(LIST_T *self, int value){
  __list_insert(self,0,value); /*prepend=add at index 0 so we just call insert with 0 as index */

}

/* Adds a new node with value at the end of the list */
void __list_append(LIST_T *self, int value){

  __list_insert(self,self->size,value); /* append= add at end so we call insert with "size" as index */

}

/*Deletes the list item at index, if it exists, and return LIST_SUCCESS; if it doesn't return LIST_ERROR */
int __list_delete(LIST_T *self, int index){

  if(index< 0 || index >= self->size ){ /* bounds checking */
    return LIST_ERROR;
  }

  if(((self->size)*2) <= self->capacity){ /* check if capacity has to shrink as it can't go 2 times more than size */
    self->capacity/=2; /* divide by a factor of 2 if that's the case */
    self->arr=realloc(self->arr, self->capacity *sizeof(int)); /* resize with the new capacity */
  }

  int i;
  /* deleting the element */
  for(i=index; i<self->size; i++){
      self->arr[i]=self->arr[i+1];
  }
  self->size--; /* decrements the size */
  return LIST_SUCCESS; /* if no error */
}

/* Returns the number of bytes allocated for the linked list */
int __list_allocated(LIST_T *self){
  return (self->capacity * sizeof(int)); /* num of bytes is the capacity times size of int in the machine (mine is 4) */

}


