#ifndef __LIST_H__
#define __LIST_H__

struct node_s {
  char *data;
  struct node_s *next;
  struct node_s *prev;
};

typedef struct node_s node;

// creation
node* createHead(char *data);
node* append(node* head, char *data);
node* prepend(node* head, char *data);

// print, one element per line
void print(node *head);

// move
node* forward(node *head);
node* backward(node *head);

// modify
node* delete(node *head);

//extra Aufgaben
void combineRing(node *head);
void splitRing(node *head);
node* destroyNoFree(node *head);
node* destroyWithFree(node *head);
node* appendList(node* head, node* other);

#endif // __LIST_H__
