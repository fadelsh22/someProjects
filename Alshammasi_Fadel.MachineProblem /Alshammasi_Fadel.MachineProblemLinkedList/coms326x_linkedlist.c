#include <stdlib.h>

#include <stddef.h>

/* including the header file of the class which contains the prototypes of the functions and the needed structures */
#include "coms326x_linkedlist.h"

/* Initializes a new list, and returns LIST_SUCess on  success, LIST_ERROR otherwise */

int init_list(LIST_T *theList){

  theList->h=NULL; /* initialize head to null */

 theList->size=0; /* initialize size to 0 */

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

 return LIST_SUCCESS; /* success if no error */

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
  return 0; /* not empty therefore return 0 */

} 

/* Returns the list value at index, if it exists and a constant LIST_ERROR otherwixe */
int __list_element(LIST_T *self, int index){
 
  LIST_ITEM_T *current= self->h; /* temp varible to keep track of the current node */

 int i=0;

 /* moving the current varible from head to the index entered */
 while(current !=NULL){
   if(i==index){
     return (current->val); /* return the value when reaching element at that index */
   }
   i++;
   current=current->next;
 }


 return LIST_ERROR; /* error othewise */

}

/* Returns the head of the list, if it exists, and LIST_ERROR otherwise */
int __list_head(LIST_T *self){

 if(self->h !=NULL){
   return(self->h->val); /* getting the value of the head and returning it */

 }
 return LIST_ERROR; /* error otherwise */
 }

/* Returns an array of integers containing the value of all components of the list except the fitst in a newly allocated int array */
int *__list_tail(LIST_T *self){

  int size=(__list_size(self)-1); /* getting the size, subtract one since head is not counted */
   int *temp=(int *)malloc(size*sizeof(int)); /* a newly allocated int array to store all elements except the first */
   
   LIST_ITEM_T *current=self->h->next; /* current is the element after head */
   int i=0;

   /* storing the elements in the temp array starting from element after head to the end */
   while(current!=NULL){
     temp[i]=current->val;
     current=current->next;
     i++;
   }

   return temp; /* return the array */

}

/* Adds a new node with value to the list at index, if index <= the list's size, if an item already exists at that index, that item and all following it are advanced in position. Returns LIST_SUCCESS if insertion was successful; LIST_ERROR otherwise */
int __list_insert(LIST_T *self, int index, int value){

  /* error if index bigger that size */
 if(index > (self->size)){
     return LIST_ERROR;
  }

 LIST_ITEM_T *current= self->h; /* temp varible to keep track of the current node */

 int i;

 /* let current be the node right before the place we want to add the new node */
 while(current!=NULL){
   if(i==(index-1)) break; /* break when reaching there */

   i++;
   current=current->next;
 }


 LIST_ITEM_T *new_node=(LIST_ITEM_T *)malloc(1*sizeof(LIST_ITEM_T)); /* allocate memory to the new node */

 /* handling case if adding a node when list is empty */
 if(__list_isEmpty(self)==1){
   new_node->val=value;
   self->h=new_node;

 }

 else{ /* if not empty */
   /* add the begining */
  if(index==0){  
    new_node->val=value;  /* provide the node with the entered value */

          new_node->next=self->h;
          self-> h=new_node;       
       } 
  /* add in the middle */
       else if(index > 0 && index< (self->size)){
            new_node->val=value;

           new_node->next=current->next;
	   current->next=new_node; 
       }
  /* add at the end */
        else if(index== (self->size)){
              new_node->val=value;
              current->next=new_node;
   }
 }

 self->size++; /* update size after adding */

 return LIST_SUCCESS ; /* if no error */

}

/* Adds a new node with value to the begining of the list */
void __list_prepend(LIST_T *self, int value){

  LIST_ITEM_T *new_node=(LIST_ITEM_T *)malloc(1*sizeof(LIST_ITEM_T)); /* allocate memory to the new node to be added at the begining */

  /* this also handles when list is empty perfectly */
  new_node->val=value; /* provide the node with entered value */
    new_node->next=self->h;
    self->h=new_node;
    self->size++; /* update size */

}

/* Adds a new node with value at the end of the list */
void __list_append(LIST_T *self, int value){

  LIST_ITEM_T *new_node=(LIST_ITEM_T *)malloc(1*sizeof(LIST_ITEM_T)); /* allocate memory to the new node to be added at the end */
    LIST_ITEM_T *current= self->h;
    
    /* move current to the end */
    while( current!=NULL && current->next!=NULL ){
      current=current->next;
    }
          
        new_node->val=value;
	new_node->next=NULL;

	/* handling case when list is empty */
	if(__list_isEmpty(self)==1){
	  self->h=new_node;
	}
	/* if not empty */
	else if(__list_isEmpty(self)==0){
	  current->next=new_node;
	}

	 self->size++;
}

/*Deletes the list item at index, if it exists, and return LIST_SUCCESS; if it doesn't return LIST_ERROR */
int __list_delete(LIST_T *self, int index){

  /* error if index is out of bounds */
  if(index >= (self->size) || self->size==0 ){
     return LIST_ERROR;
  }

  LIST_ITEM_T *current= self->h;
  int i=0;

  /* delete at the begining */
    if(index==0){
       self->h=current->next;
       free(current); /* free what was deleted */
     } 

    /* delete in the middle */
    else if(index > 0 && index< ((self->size)-1)){
      /* move current to the place right before the node to be deleted */
          while(current!=NULL){
             if(i==(index-1)) break;

              i++;
              current=current->next;
             } 

      LIST_ITEM_T *toBeDeleted= current->next;
      current->next=toBeDeleted->next;
      free(toBeDeleted); /* freeing the deleted node */
     }

    /* delete at the end */
    else if(index== ((self->size)-1)){
      /* move current to the node right before what we want to delete */
        while(current!=NULL){
            if(i==(index-1)) break;

             i++;
           current=current->next;
      } 

	current->next=NULL; /* setting deleted node to null */
	free(current->next); /* freeing it */ 
     
     }


    self->size--; /* decrements the size */
  return LIST_SUCCESS; /* if no error */

}

/* Returns the number of bytes allocated for the linked list */
int __list_allocated(LIST_T *self){
  return (self->size *sizeof(int));

}
