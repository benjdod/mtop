#include <stdio.h>
#include "bst.h"

void onvalue(bst_value_t val) {
    printf("[%d] %s (%d)\n", val.pid, val.cmd, val.priority);
}

int main () {
    bst_t tree = bst_init();

    some_proc_t procs[] =  {
         {1, "init", 1},
         {6, "init", 13},
         {9, "init", 100},
         {17, "bash", 70},
         {18, "grep", 75}
    };

    for (int i = 0; i < sizeof(procs) / sizeof(procs[0]); i++) {
        bst_insert(&tree, procs[i]);
    }

    bst_remove(&tree, procs[4]);

    //printf("tree %s %d\n", (bst_contains(&tree, 6)) ? "has" : "doesn't have", 6);
    bst_inorder(&tree, &onvalue);
    putchar('\n');

    bst_destroy(&tree);
    return 0;
}