#include <stdio.h>

#include "list.h"

int main (int argc, char** argv) {
  // create a linked list containg three elements
  node* head = createHead("first element");
  append(head, "second element");
  head = prepend(head, "before first"); 
  append(head, "third element");
  append(head, "fourth element");
  head = prepend(head, "real first");
  append(head, "last element");

  printf("\nOrder:\n");
  print(head);
  
  node* otherHead = createHead("first other");
  append(otherHead, "second other");
  append(otherHead, "third other");

  printf("\nOrder other:\n");
  print(otherHead);


//TEST memory
  //head = destroyNoFree(head);
  //head = destroyWithFree(head);

//TEST append LIST to LIST
  node* newHead; 
  newHead = appendList(head, otherHead);


//TEST erstelle RINGLISTE
/*
  combineRing(head);
  print(head);
  
  splitRing(head);
  print(head);
*/

 /* //delelte TEST
  printf("\nDelete real first:\n");
  head = delete(head);
  print(head);

  for(int i=0; i<6; i++){
      if(i==1)
          head = forward(head);
      if(i==4)
          head = forward(head);
      head = delete(head);
      print(head);
  }
/**/ //DELETE TEST ENDE

  printf("list at end:\n");
  //printf(newHead);
  if(head==NULL){
      printf("Liste leer\n");
      return 0;
  }else{
    print(newHead);
  }
  return 0;
}
