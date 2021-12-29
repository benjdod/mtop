#ifndef CMTOP_PROCLIST_H
#define CMTOP_PROCLIST_H

#include "common.h"
#include "xutil.h"
#include "proctypes.h"

typedef struct procnode_t_ procnode_t;
typedef struct proclist_t_ proclist_t;
typedef struct proclist_cur_t_ proclist_cur_t;

struct procnode_t_ {
    procinfo_t value;
    procnode_t 
        *next,
        *prev;
};

struct proclist_cur_t_ {
    proclist_t* list;
    procnode_t* current;
    u8 pos;
};

struct proclist_t_ {
    procnode_t
        *head,
        *tail;
    size_t size;
};


proclist_t proclist_init();
void proclist_destroy(proclist_t* list);

procinfo_t* proclist_append(proclist_t* list, procinfo_t info);
procinfo_t* proclist_prepend(proclist_t* list, procinfo_t info);
procinfo_t* proclist_insert(proclist_t* list, procinfo_t info);
procinfo_t* proclist_find(proclist_t* list, pid_t pid);
void proclist_remove(proclist_t* list, pid_t pid);
void proclist_foreach (proclist_t* list, void (*on_value)(procinfo_t*));

proclist_cur_t pl_cur_init(proclist_t* list);
u8 pl_cur_hasnext(proclist_cur_t* cur);
u8 pl_cur_hasprev(proclist_cur_t* cur);
u8 pl_cur_eq(proclist_cur_t* a, proclist_cur_t* b);
procinfo_t* pl_cur_at(proclist_cur_t* cur);
procinfo_t* pl_cur_next(proclist_cur_t* cur);
procinfo_t* pl_cur_prev(proclist_cur_t* cur);
procinfo_t* pl_cur_first(proclist_cur_t* cur);
procinfo_t* pl_cur_last(proclist_cur_t* cur);
#define PL_CURSHIFT_LEFT 0
#define PL_CURSHIFT_RIGHT 1
#define PL_CURVAL(cursorptr) ((cursorptr)->current != NULL ? &(cursorptr)->current->value : NULL)
#define PL_CUR_FOREACH(cursor) for (pl_cur_next(cursor) ; (cursor)->current != NULL; pl_cur_next(cursor))
void pl_cur_remove(proclist_cur_t* cur, u8 shift_dir);    // warning! mutates underlying list

#endif