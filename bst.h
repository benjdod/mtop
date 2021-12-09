#ifndef _CMTOP_BST_H
#define _CMTOP_BST_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    int pid;
    char *cmd;
    int priority;
} some_proc_t;

typedef some_proc_t bst_value_t;

typedef struct bstnode_t bstnode_t;

struct bstnode_t {
    bst_value_t value;
    bstnode_t 
        *left,
        *right,
        *parent;
};

typedef struct {
    bstnode_t *head;
    size_t num_elts;
} bst_t;

bst_t bst_init();
void  bst_destroy(bst_t *tree);
void bst_insert(bst_t *tree, bst_value_t value);
void bst_inorder(bst_t *tree, void (*on_value)(bst_value_t));
int bst_contains    (bst_t *tree, bst_value_t value);
void bst_remove     (bst_t *tree, bst_value_t value);

#define BST_COMPARE(a,b) ((b.pid) - (a.pid))
//#define BST_EQUALS(a,b) ((a) == (b))

#endif