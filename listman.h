#ifndef LISTMAN_H
#define LISTMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <utilities.h>

typedef struct listElement {
  struct listElement *prev;
  struct listElement *next;
  void *data;
}listElement;

typedef class listman {
private:
  long int numElements;
  listElement anchor;

protected:

public:
    listman();
    ~listman();

    //basic management
    long int length();
    int isEmpty();

    //list traversal
    listElement * first();
    listElement * last ();
    listElement * next(listElement *current);
    listElement * prev(listElement *current);

    //element insertion
    int insertBefore(listElement *position, void *datapoint);
    int insertAfter (listElement *position, void *datapoint);
    int append (void *datapoint);
    int prepend(void *datapoint);

    //element deletion
    void unlink(listElement *);
    void unlinkAll();
}listman;

#endif
