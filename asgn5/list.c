
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
// structs --------------------------------------------------------------------
// private Node type
typedef struct NodeObj *Node;

// private NodeObj type
typedef struct NodeObj {
    ListElement data;
    Node prev;
    Node next;
} NodeObj;

// private ListObj type
typedef struct ListObj {
    Node front;
    Node back;
    Node cursor;
    int index;
    int length;
} ListObj;
// Constructors-Destructors ---------------------------------------------------
// newNode()
// Returns reference to new Node object. Initializes next and data fields.
Node newNode(ListElement data) {
    Node N = malloc(sizeof(NodeObj));
    assert(N != NULL);
    N->data = strdup(data);
    N->next = N->prev = NULL;
    return (N);
}

// freeNode()
// Frees heap memory pointed to by *pN, sets *pN to NULL.
void freeNode(Node *pN) {
    if (pN != NULL && *pN != NULL) {
        free((void *) (*pN)->data);
        free(*pN);
        *pN = NULL;
    }
}

// newList()
// Returns reference to new empty List object.
List newList(void) {
    List L;
    L = malloc(sizeof(ListObj));
    assert(L != NULL);
    L->front = L->back = L->cursor = NULL;
    L->length = 0;
    L->index = -1;
    return (L);
}

// freeList()
// Frees all heap memory associated with List *pL, and sets *pL to NULL.
void freeList(List *pL) {
    if (pL != NULL && *pL != NULL) {
        Node N = (*pL)->front;
        while (N != NULL) {
            Node M = N;
            N = N->next;
            freeNode(&M);
        }
        (*pL)->front = NULL;
        (*pL)->back = NULL;
        (*pL)->cursor = NULL;
        free(*pL);
        *pL = NULL;
    }
}

// Access functions -----------------------------------------------------------

// length()
// Returns the length of the List.
int length(List L) {
    if (L == NULL) {
        printf("List Error: calling length() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return (L->length);
}
// index()
// Returns the index of the cursor element. If not defined, return -1.
int indexs(List L) {
    if (L == NULL) {
        printf("List Error: calling index() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    if (L->index < 0 || L->index >= length(L)) {
        return -1;
    }
    return (L->index);
}

// front()
// Returns the value at the front of L.
// Pre: !isEmpty(L)
ListElement front(List L) {
    if (L == NULL) {
        printf("List Error: calling front() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return (L->front->data);
}

// back()
// Return the value at the back of L.
// Pre: !isEmpty(L)
ListElement back(List L) {
    if (L == NULL) {
        printf("List Error: calling back() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    return (L->back->data);
}
// get()
// Return the value at the index of the cursor element.
// Pre: length()>0, index()>=0
ListElement get(List L) {
    if (L == NULL) {
        printf("List Error: calling get() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (indexs(L) < 0) {
        printf("List Error: calling get() on an index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    //printf("get 1\n");
    return (L->cursor->data);
    //printf("get 1\n");
}

// isEmpty()
// Returns true if Q is empty, otherwise returns false.
bool isEmpty(List L) {
    if (L == NULL) {
        printf("List Error: calling isEmpty() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return (L->length == 0);
}

int isInList(List L, const char *item) {
    Node N = L->front;
    while (N != NULL) {
        // fprintf(stderr, "Comparing: %s and %s\n", N->data, item);
        if (strcmp(N->data, item) == 0) {
            return 1;
        }
        N = N->next;
    }
    freeNode(&N);
    return 0;
}

// Manipulation procedures ----------------------------------------------------

// clear()
// Resets L to its original empty state.
void clear(List L) {
    if (isEmpty(L)) {
        return;
    }
    //freeList(L);
    while (L->front != NULL) {
        deleteFront(L);
    }
}
// set()
// Overwrites the cursor element’s data with x
// Pre: length()>0, index()>=0
void set(List L, ListElement x) {
    if (L->length < 0 || L->index < 0) {
        printf("List Error: calling set() with out of bounds parameter.\n");
        exit(EXIT_FAILURE);
    }
    L->cursor->data = strdup(x);
}

// moveFront()
// If L is non-empty, sets cursor under the front element, otherwise does nothing.
void moveFront(List L) {
    if (isEmpty(L)) {
        return;
    }
    L->cursor = L->front;
    //printf("moveFront\n");
    L->index = 0;
}

// moveBack()
// If L is non-empty, sets cursor under the back element, otherwise does nothing.
void moveBack(List L) {
    if (isEmpty(L)) {
        return;
    }

    L->cursor = L->back;
    L->index = L->length - 1;
}
// movePrev()
// If cursor is defined and not at front, move cursor one
// step toward the front of L; if cursor is defined and at
// front, cursor becomes undefined; if cursor is undefined
// do nothing
void movePrev(List L) {
    if (isEmpty(L)) {
        printf("List Error: calling movePrev() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    // if(L->index != -1) {
    //    Node N = L->cursor;
    //    if (N->prev != NULL) {
    //       printf("\ncurrent val:%d", L->cursor->data);
    //       N = N->prev;
    //       L->cursor = N;
    //       L->index--;
    //       printf("\ncurrent val:%d", L->cursor->data);

    //    }
    //    else {
    //       L->index = -1;
    //       L->cursor = NULL;
    //    }
    // }

    if (L->index > 0) { //we are not at the front
        L->index--;
        L->cursor = L->cursor->prev;

    } else {
        L->index = -1;
        L->cursor = NULL;
    }
}

// moveNext()
// If cursor is defined and not at back, move cursor one
// step toward the back of L; if cursor is defined and at
// back, cursor becomes undefined; if cursor is undefined
// do nothing
void moveNext(List L) {
    if (L->index != -1) {
        Node N = L->cursor;
        if (N->next != NULL) {
            N = N->next;
            L->cursor = N;
            L->index++;
        } else {
            L->index = -1;
        }
    }
}

// prepend()
// Insert new element into L. If L is non-empty,
// insertion takes place before front element.
void prepend(List L, ListElement x) {

    Node N = newNode(x);

    if (isEmpty(L)) {
        //printf("here 8.\n");
        L->front = N;
        L->back = N;
        L->length++;
        //printf("here 5.\n");
        return;
    }
    N->next = L->front;
    if (L->front != NULL) {
        L->front->prev = N;
    }
    L->front = N;
    //printf("here 2.\n");

    //printf("here 3.\n");
    L->length++;
    if (L->index != -1) {
        L->index++;
    }

    //printf("here 4.\n");
}

// append()
// Insert new element into L. If L is non-empty,
// insertion takes place after back element.
void append(List L, ListElement x) {
    Node N = newNode(x);
    //printf("N data: %d.\n", N->data);
    if (isEmpty(L)) {
        //printf("here 8.\n");
        L->front = N;
        L->back = N;
        L->length++;
        //printf("here 5.\n");
        return;
    }
    L->back->next = N;
    N->prev = L->back;
    L->back = N;
    //printf("here 6.\n");
    L->length++;
}

// insertBefore()
// Insert new element before cursor.
// Pre: length()>0, index()>=0
void insertBefore(List L, ListElement x) {
    if (L->cursor == L->front) {
        prepend(L, x);
        return;
    }
    if (isEmpty(L)) {
        printf("List Error: calling insertBefore() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length <= 0) {
        printf("List Error: length is less than or equal to 0\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        printf("List Error: index is less than 0\n");
        exit(EXIT_FAILURE);
    }
    // Node N = newNode(x);
    // Node P = L->cursor->prev;
    // Node C = L->cursor;
    // P->next = N;
    // N->prev = P;
    // N->next = C;
    // C->prev = N;

    // // if (N->prev != NULL) {
    // //    N->prev->next = N;
    // // }
    // L->length++;
    // L->index++;

    Node N = newNode(x);
    L->cursor->prev->next = N;
    N->prev = L->cursor->prev;
    L->cursor->prev = N;
    N->next = L->cursor;
    L->length++;
    L->index++;
}

// insertAfter()
// Insert new element after cursor.
// Pre: length()>0, index()>=0
void insertAfter(List L, ListElement x) {
    if (L->cursor == L->back) {
        append(L, x);
        return;
    }

    if (isEmpty(L)) {
        printf("List Error: calling insertAfter() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: length is less than 0\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        printf("List Error: index is less than 0\n");
        exit(EXIT_FAILURE);
    }
    Node N = newNode(x);
    N->prev = L->cursor;
    N->next = L->cursor->next;
    L->cursor->next = N;
    N->next->prev = N;

    // L->cursor->next->prev = N;
    // N->next = L->cursor->next;
    // L->cursor->next = N;
    // N->prev = L->cursor;

    if (N->next != NULL) {
        N->next->prev = N;
    }
    L->length++;
}

// deleteFront()
// Delete the front element. Pre: length()>0
void deleteFront(List L) {
    if (L->length < 0) {
        printf("List Error: length is less than 0\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        return;
    }
    if (L->cursor == L->front) {
        L->cursor = NULL;
        L->index = -1;
    }

    Node N = L->front;
    L->front = L->front->next;
    if (L->front == NULL) {
        L->back = NULL;

    } else {
        L->front->prev = NULL;
    }
    L->index--;
    L->length--;
    freeNode(&N);
    N = NULL;
}

// deleteBack()
// Delete the back element. Pre: length()>0
void deleteBack(List L) {
    if (L->length < 0) {
        printf("List Error: length is less than 0\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        printf("List Error: calling deleteBack() on NULL List reference\n");
        return;
    }

    Node N = L->back;
    if (L->cursor == N) {
        L->cursor = NULL;
        L->index = -1;
    }
    if (N->prev != NULL) {
        N->prev->next = NULL;
    }
    L->back = N->prev;
    freeNode(&N);
    N = NULL;
    L->length--;
}

// delete()
// Delete cursor element, making cursor undefined.
// Pre: length()>0, index()>=0
void deleteCursor(List L) {
    if (L->length < 0) {
        printf("List Error: length is less than 0\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        printf("List Error: index is less than 0\n");
        exit(EXIT_FAILURE);
    }

    if (isEmpty(L)) {
        printf("List Error: calling delete() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    // Node N = L->cursor;
    // if(L->cursor->prev && L->cursor->next){
    //    L->cursor->prev->next = L->cursor->next;
    //    L->cursor->next->prev = L->cursor->prev;
    // }
    // printf("%d\n", L->cursor->next->data);
    // L->cursor = NULL;
    // L->index = -1;
    // Node N = L->cursor;
    // if (N->next == NULL) {
    //    //printf("here er\n");
    //    freeNode(&L->cursor);
    //    L->cursor = NULL;
    //    L->index = -1;
    //    L->length--;
    //    return;
    // }
    if (L->cursor == L->back) {
        deleteBack(L);
        return;
    }
    if (L->cursor == L->front) {
        deleteFront(L);
        return;
    }
    //printf("here 7\n");
    if (L->cursor->next != NULL) {
        L->cursor->next->prev = L->cursor->prev;
    }

    //printf("here 323\n");
    if (L->cursor->prev != NULL) {
        L->cursor->prev->next = L->cursor->next;
    }

    ///printf("here 43\n");
    freeNode(&L->cursor);
    L->cursor = NULL;
    L->index = -1;
    L->length--;
}

// Other operations -----------------------------------------------------------

// printList()
// Prints to the file pointed to by out, a
// string representation of L consisting
// of a space separated sequence of integers,
// with front on left.
void printList(FILE *out, List L) {
    Node N = L->front;
    while (N != NULL) {
        fprintf(out, "%s ", N->data);
        N = N->next;
    }
    //printf("\n");
    freeNode(&N);
}
