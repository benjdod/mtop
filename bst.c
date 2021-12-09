#include "bst.h"

bstnode_t* bstnode_make(bst_value_t value, bstnode_t* left, bstnode_t* right, bstnode_t* parent) {
    bstnode_t* node = malloc(sizeof(bstnode_t));
    node->value = value;
    node->left = left;
    node->right = right;
    node->parent = parent;
    return node;
}

void bstnode_destroyall(bstnode_t* node) {
    if (node == NULL) return;

    bstnode_destroyall(node->left);
    bstnode_destroyall(node->right);
    free(node);
}

void bstnode_destroy(bstnode_t* node) {
    free(node);
}

bst_t bst_init() {
    bst_t b;
    b.head = NULL;
    b.num_elts = 0;
    return b;
}

void bst_insert(bst_t* tree, bst_value_t value) {

    if (tree->head == NULL) {
        tree->head = bstnode_make(value, NULL, NULL, NULL);
        tree->num_elts = 1;
        return;
    }

    bstnode_t* current = tree->head;
    bstnode_t* previous = tree->head;
    bstnode_t** leafptr = NULL;

    // one or more elements
    while (current != NULL) {

        previous = current;
        leafptr = (BST_COMPARE(value, current->value) >= 0) 
            ? &current->left
            : &current->right;
        current = *leafptr;
    }

    *leafptr = bstnode_make(value, NULL, NULL, previous);
    tree->num_elts++;
}     

bstnode_t* bstnode_findchild(bstnode_t* node, bst_value_t value) {

    if (node == NULL) return NULL;

    bstnode_t* current = node;

    while (current != NULL) {
#ifdef BST_EQUALS
        if (BST_EQUALS(value, current->value)) return current;

#else
        if (BST_COMPARE(value, current->value) == 0) return current;
#endif

        current = (BST_COMPARE(value, current->value) >= 0)
            ? current->left
            : current->right;
        
    }

    return NULL;
}


bstnode_t* bst_findnode(bst_t* tree, bst_value_t value) {
    return bstnode_findchild(tree->head, value);
}

int bst_contains(bst_t* tree, bst_value_t value) {
    return (bst_findnode(tree, value) != NULL) ? 1 : 0;
}


int bstnode_removechild(bstnode_t* head, bst_value_t value) {

    if (head == NULL) return 0;

    bstnode_t* node = bstnode_findchild(head, value);

    if (node == NULL) return 0;

    bstnode_t* parent = node->parent;
    bstnode_t** pcp = (node == parent->left)
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
        bstnode_t* succ = node->right;

		// get successor
		succ = node->right;
		while (succ->left != NULL) succ = succ->left; 

        //printf("replacing with successor: %d\n", succ->value);

		node->value = succ->value;
		bstnode_removechild(node->right, succ->value);
    }

    bstnode_destroy(node);
    return 1;
}

void bst_remove(bst_t* tree, bst_value_t value) {
    if (bstnode_removechild(tree->head, value)) {
        tree->num_elts--;
    }
    return;
}



static void bst_inorder_node(bstnode_t* node, void (*on_value)(bst_value_t)) {
    if (node == NULL) return;

    bst_inorder_node(node->left, on_value);
    on_value(node->value);
    bst_inorder_node(node->right, on_value);
}

static void bst_preorder_node(bstnode_t* node, void (*on_value)(bst_value_t)) {
    if (node == NULL) return;

    on_value(node->value);
    bst_preorder_node(node->left, on_value);
    bst_preorder_node(node->right, on_value);
}

static void bst_postorder_node(bstnode_t* node, void (*on_value)(bst_value_t)) {
    if (node == NULL) return;

    bst_postorder_node(node->left, on_value);
    bst_postorder_node(node->right, on_value);
    on_value(node->value);
}


void bst_inorder(bst_t* tree, void (*on_value)(bst_value_t)) {
    bst_inorder_node(tree->head, on_value);
}

void bst_preorder(bst_t* tree, void (*on_value)(bst_value_t)) {
    bst_preorder_node(tree->head, on_value);
}

void bst_postorder(bst_t* tree, void (*on_value)(bst_value_t)) {
    bst_postorder_node(tree->head, on_value);
}

void bst_destroy(bst_t *tree) {
    bstnode_destroyall(tree->head);
    tree->head = NULL;
    tree->num_elts = 0;
}