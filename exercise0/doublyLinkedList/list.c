#include "list.h"

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

// creation
node* createHead(char *data) {
  node* head = malloc(sizeof(node)); // acquire memomry for head node
  head->data = data;                // set data pointer
  head->next = NULL;                // set next to NULL to signal it is not valid (this is the last element)
  //ab hier mein code
  head->prev = NULL;                //set prev to NULL to signal first element

  return head;
}

// helper function, only visible in this file
static node* findLast(node *head) {
  if (head->next == NULL) { // no next node, this must be the last
    //printf("%s%s\n","findLast: ", head->data);
    return head;
  } else {
    return findLast(head->next); // recursive function call
  }
}

// create new node for data and append to the end of the list
node* append(node* head, char *data) {
  node *newNode = createHead(data);//neue node erstellen
  node *last = findLast(head);//letzte node finden
  last->next = newNode;//letzte node auf next = neue node

  //my code
  newNode->prev = last;  
   
  return head;
}

// helper function, only visible in this file
static node* findFirst(node *head) {
  if(head->prev == NULL){
    //printf("%s%s\n","findFirst: ",head->data);
    return head;
  }else{
    return findFirst(head->prev);
  }
}

// prepend on the first position -------------------- changed
node* prepend(node *head, char *data) {
  node *newNode = createHead(data);//neue node erstellen
  newNode->next = findFirst(head); //finde erstes element 
  //ab hier soll head die newNode sein, ist es aber nicht
  
  head->prev = newNode;
  //*head = newNode;

  return newNode; 
}

// print, one element per line
static void printPrime(int i, node *head) {
  int j = 40; //anzahl an maximalen durchläufen für RINGLISTE
  node* temp = head;  
  while(j>0 && head != NULL){
     printf("%d: %s->\n", i++, head->data);
//     head = backward(head);//rückwärts aufzählen
     head = forward(head);//vorwärts aufzählen 
     j--;
  }

  head = temp; 
/*  printf("%d: %s\n", i, head->data);

  if (head->next != NULL) {
    printPrime(i + 1, head->next);
  }
  */
}

void print(node *head) {
  if(head != NULL){
  //printPrime(0, findFirst(head));
  }
  printPrime(0, head);//zahl gibt an wie oft RINGLISTE maximal durchlaufen wird
  printf("---------\n");
}

// move
node* forward(node *head) {
  if (head->next != NULL){
    return head->next;
  } else { // return NULL to signal that move is not possible
    return NULL;
  }
}

node* backward(node *head) {
  if(head->prev !=NULL){
      return head->prev;
  }else{
      return NULL;
  }//doneeee
}

// modify
// remove the given element, merge the list parts and
// return the element before the one we just removed (if it exists)
//     or the element after  the one we just removed (if it exists, but no previous element exists)
//     or NULL (if this was the only elemnt)
node* delete(node *head) {

  if(head->prev == NULL && head->next == NULL){//einziges element
    //printf("einziges element\n");
    return NULL;
  }
  if(head->prev == NULL){//1. position
    //printf("1.Stelle\n");
    node* temp = head->next;
    head->next = NULL;
    temp->prev = NULL;
    return temp;
  }
  if(head->next == NULL){//letzte position
    //printf("letzte Stelle\n");
    node* temp = head->prev;
    head->prev = NULL;
    temp->next = NULL;
    return temp;
  }
  //printf("mittendrin\n");
  node* tempPrev = head->prev;
  node* tempNext = head->next;
  head->prev = NULL;
  head->next = NULL;
  tempPrev->next = tempNext;
  tempNext->prev = tempPrev;
  return tempPrev;
}

//erstelle RINGliste
void combineRing(node *head){
  node* last = findLast(head);
  node* first = findFirst(head);
  first->prev = last;
  last->next = head;

}

//splitte RINGLISTE
void splitRing(node *head){
  srand(time(NULL));
  int i = rand() % 40;
  for(; i>0;i--){ //suche zufälligen knoten
      head = forward(head);
  }
  
  node* temp = backward(head);
  temp->next = NULL;
  head->prev = NULL;

}

node* destroyNoFree(node *head){
  node* current = findFirst(head); 
  node* temp = current;


  while (current != NULL){
    temp = current->next;
    free(current);
    current = temp;
  }
  return NULL;  
}

node* destroyWithFree(node *head){
  node* current = findFirst(head); 
  node* temp = current;

  while (current != NULL){
    temp = current->next;
    if(current->data != NULL){
        current->data = NULL;
        //free(current->data);
    }
    free(current);
    current = temp;
  }

  return NULL;
}

node* appendList(node *head, node *other){
  node* temp = findLast(head);
  temp->next = findFirst(other);
  temp = findFirst(other);
  temp->prev = findLast(head);
  


  return findFirst(head);
}
 
