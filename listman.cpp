#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "listman.h"

listman::listman() {
  numElements = 0;
  anchor.prev = NULL;
  anchor.next = NULL;
  anchor.data = NULL;
}

long int listman::length() {
  return numElements;
}

int listman::isEmpty() {
  return (numElements == 0)?1:0;
}

listElement * listman::first() {
  return anchor.next;
}

listElement * listman::last() {
  return anchor.prev;
}

listElement * listman::next(listElement *current) {
  if(numElements <= 1) return NULL;
  return current->next;
}

listElement * listman::prev(listElement *current) {
  if(numElements <= 1) return NULL;
  return current->prev;
}

int listman::insertBefore(listElement *position, void *datapoint) {
  listElement *newElem = (listElement *)malloc(sizeof(listElement));
  newElem->data = datapoint;

  if(numElements == 0) {
    anchor.prev = anchor.next = newElem;
    newElem.prev = newElem.next = &anchor;
  }
  else {
    newElem->prev = position->prev;
    newElem->next = position;
    position->prev->next = newElem;
    position->prev = newElem;
  }

  numElements++;
  return TRUE;
}

int listman::insertAfter(listElement *position, void *datapoint) {
  listElement *newElem = (listElement *)malloc(sizeof(listElement));
  newElem->data = datapoint;

  if(numElements == 0) {
    anchor.prev = anchor.next = newElem;
    newElem.prev = newElem.next = &anchor;
  }
  else {
    newElem->next = position->next;
    newElem->prev = position;
    (position->next)->prev = newElem;
    position->next = newElem;
  }

  numElements++;
  return TRUE;
}

int listman::append(void *datapoint) {
  return insertAfter(last(),datapoint);
}

int listman::prepend(void *datapoint) {
  return insertBefore(first(),datapoint);
}

void listman::unlink(listElement *delElem) {
  if(!isEmpty) {
    (delElem->prev)->next = delElem->next;
    (delElem->next)->prev = delElem->prev;
    numElements--;
    free(delElem);
  }
  delElem = NULL;
}

void listman::unlinkAll() {
    listElement *delElem;
    while (!isEmpty) {
      unlink(first());
    }
}

listman::~listman() {
  unlinkAll();
}
