#include "procbst.h"

procbst_node_t* node_make(bst_value_t value, procbst_node_t* left, procbst_node_t* right, procbst_node_t* parent) {
    procbst_node_t* node = malloc(sizeof(procbst_node_t));
    node->value = value;
    node->left = left;
    node->right = right;
    node->parent = parent;
    return node;
}

void node_destroyall(procbst_node_t* node) {
    if (node == NULL) return;

    node_destroyall(node->left);
    node_destroyall(node->right);
    free(node);
}

void node_destroy(procbst_node_t* node) {
    free(node);
}

procbst_t procbst_init() {
    procbst_t b;
    b.head = NULL;
    b.num_elts = 0;
    return b;
}

procinfo_t* procbst_insert(procbst_t* tree, bst_value_t value) {

    if (tree->head == NULL) {
        tree->head = node_make(value, NULL, NULL, NULL);
        tree->num_elts = 1;
        return &(tree->head->value);
    }

    procbst_node_t* current = tree->head;
    procbst_node_t* previous = tree->head;
    procbst_node_t** leafptr = NULL;

    // one or more elements
    while (current != NULL) {

        previous = current;
        leafptr = (PROCBST_COMPARE(value, current->value) >= 0) 
            ? &current->left
            : &current->right;
        current = *leafptr;
    }

    *leafptr = node_make(value, NULL, NULL, previous);
    tree->num_elts++;

    return &(*leafptr)->value;
}     

procbst_node_t* node_findchild(procbst_node_t* node, bst_value_t value) {

    if (node == NULL) return NULL;

    procbst_node_t* current = node;

    while (current != NULL) {
#ifdef BST_EQUALS
        if (BST_EQUALS(value, current->value)) return current;

#else
        if (PROCBST_COMPARE(value, current->value) == 0) return current;
#endif

        current = (PROCBST_COMPARE(value, current->value) >= 0)
            ? current->left
            : current->right;
        
    }

    return NULL;
}

int procbst_contains(procbst_t* tree, bst_value_t value) {
    return (node_findchild(tree->head, value) != NULL) ? 1 : 0;
}

bst_value_t* procbst_find(procbst_t* tree, pid_t pid) {
    bst_value_t p;
    p.pid = pid;

    return (&node_findchild(tree->head, p) -> value);
}


int node_removechild(procbst_node_t* head, bst_value_t value) {

    if (head == NULL) return 0;

    procbst_node_t* node = node_findchild(head, value);

    if (node == NULL) return 0;

    procbst_node_t* parent = node->parent;
    procbst_node_t** pcp = (node == parent->left)
            ? &parent->left
            : &parent->right;

    //printf("replacing node %d @ %p\n", node->value, node);
    //printf("%p ^ %d ^ %p\n", node->left, node->value, node->right);

    if (node->left == NULL && node->right == NULL) {    // no children
        *pcp = NULL;
    } else if (node->left != NULL || node->right != NULL) { // 1 child

        if (node->left != NULL) {
            //printf("setting parent leaf to left node\n");
            *pcp = node->left;
        } else {
            //printf("setting parent leaf to right node\n");
            *pcp = node->right;
        }

        *pcp = (node->left != NULL)
            ? node->left
            : node->right;
    } else {   // 2 children
        procbst_node_t* succ = node->right;

		// get successor
		succ = node->right;
		while (succ->left != NULL) succ = succ->left; 

        //printf("replacing with successor: %d\n", succ->value);

		node->value = succ->value;
		node_removechild(node->right, succ->value);
    }

    node_destroy(node);
    return 1;
}

void procbst_remove(procbst_t* tree, pid_t pid) {
    // XXX: this just exploits the guaranteed functionality of 
    // PROCBST_COMPARE. fine for now but not robust or simple.
    bst_value_t value;
    value.pid = pid;

    if (node_removechild(tree->head, value)) {
        tree->num_elts--;
    }
    return;
}



static void node_inorder(procbst_node_t* node, void (*on_value)(bst_value_t*)) {
    if (node == NULL) return;

    node_inorder(node->left, on_value);
    on_value(&node->value);
    node_inorder(node->right, on_value);
}

static void node_preorder(procbst_node_t* node, void (*on_value)(bst_value_t)) {
    if (node == NULL) return;

    on_value(node->value);
    node_preorder(node->left, on_value);
    node_preorder(node->right, on_value);
}

static void node_postorder(procbst_node_t* node, void (*on_value)(bst_value_t)) {
    if (node == NULL) return;

    node_postorder(node->left, on_value);
    node_postorder(node->right, on_value);
    on_value(node->value);
}


void procbst_inorder(procbst_t* tree, void (*on_value)(bst_value_t*)) {
    node_inorder(tree->head, on_value);
}

void procbst_preorder(procbst_t* tree, void (*on_value)(bst_value_t)) {
    node_preorder(tree->head, on_value);
}

void procbst_postorder(procbst_t* tree, void (*on_value)(bst_value_t)) {
    node_postorder(tree->head, on_value);
}

procbst_cursor_t procbst_cursor_init(procbst_t* tree) {
    procbst_cursor_t c;
    c.tree = tree;
    c.current = NULL;
    return c;
}

const procinfo_t* procbst_cursor_first(procbst_cursor_t* cursor) {
    if (cursor->tree == NULL) return NULL;

	if (cursor->current == NULL) cursor->current = cursor->tree->head;

    // climb up to the root
	while (cursor->current->parent != NULL) cursor->current = cursor->current->parent;

    // descend to first elt
	while (cursor->current->left != NULL) cursor->current = cursor->current->left;
	return &cursor->current->value;
}

const procinfo_t* procbst_cursor_next(procbst_cursor_t* cursor) {

	if (cursor->current == NULL) return procbst_cursor_first(cursor);

    if (cursor->current->right != NULL) {
        cursor->current = cursor->current->right;
        while (cursor->current->left != NULL) {cursor->current = cursor->current->left;}
    } else {
        if (cursor->current->parent->right == cursor->current) {
            cursor->current = NULL;
            return NULL;
        }
        cursor->current = cursor->current->parent;
    }

    return &cursor->current->value;
}

const procinfo_t* procbst_cursor_prev(procbst_cursor_t* cursor) {

	if (cursor->current == NULL) return procbst_cursor_last(cursor);

    if (cursor->current->left != NULL) {
        cursor->current = cursor->current->left;
        while (cursor->current->right != NULL) {cursor->current = cursor->current->right;}
    } else {
        if (cursor->current->parent->left == cursor->current) {
            cursor->current = NULL;
            return NULL;
        }
        cursor->current = cursor->current->parent;
    }

    return &cursor->current->value;
}


const procinfo_t* procbst_cursor_last(procbst_cursor_t* cursor) {
	if (cursor->current == NULL) cursor->current = cursor->tree->head;
	while (cursor->current->parent != NULL) cursor->current = cursor->current->parent;
	while(cursor->current->right != NULL) cursor->current = cursor->current->right;
	return &cursor->current->value;
}

int procbst_cursor_eq(procbst_cursor_t a, procbst_cursor_t b) {
    return (a.current == b.current);
}

void procbst_dynamic_remove(procbst_cursor_t* cursor) {
    pid_t pid = cursor->current->value.pid;
    procbst_cursor_next(cursor);
    procbst_remove(cursor->tree, pid);
}

void procbst_destroy(procbst_t *tree) {
    node_destroyall(tree->head);
    tree->head = NULL;
    tree->num_elts = 0;
}