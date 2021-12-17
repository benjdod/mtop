#ifndef _CMTOP_BST_H
#define _CMTOP_BST_H

#include <stdlib.h>
#include <stdint.h>
#include "proctypes.h"

typedef procinfo_t bst_value_t;

typedef struct procbst_node_t procbst_node_t;

struct procbst_node_t {
    bst_value_t value;
    procbst_node_t 
        *left,
        *right,
        *parent;
};

typedef struct {
    procbst_node_t *head;
    size_t num_elts;
} procbst_t;

typedef struct {
    procbst_t *tree;
    procbst_node_t *current;
} procbst_cursor_t;

procbst_t procbst_init  ();
void  procbst_destroy   (procbst_t *tree);
procinfo_t* procbst_insert     (procbst_t *tree, bst_value_t value);
procinfo_t* procbst_find(procbst_t *tree, pid_t pid);
void procbst_inorder    (procbst_t *tree, void (*on_value)(bst_value_t*));
int procbst_contains    (procbst_t *tree, bst_value_t value);
void procbst_remove     (procbst_t *tree, pid_t pid);
void procbst_destroy    (procbst_t *tree);
procbst_cursor_t procbst_cursor_init(procbst_t* tree);
const procinfo_t* procbst_cursor_next(procbst_cursor_t* cursor);
void procbst_dynamic_remove(procbst_cursor_t* cursor);

#define PROCBST_COMPARE(a,b) ((b.pid) - (a.pid))
//#define BST_EQUALS(a,b) ((a) == (b))

#endif