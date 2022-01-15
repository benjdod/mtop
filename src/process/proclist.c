/* Copyright (c) 2022 Ben Dod 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE. */

#include "proclist.h"

// node methods

static procnode_t* makenode(procnode_t* prev, procinfo_t info, procnode_t* next) {
    procnode_t* out = (procnode_t*) x_malloc(1, sizeof(procnode_t));
    out->value = info;
    out->next = next;
    out->prev = prev;
    return out;
}

static void freenode(procnode_t* node) {
    x_free(node);
}

// list methods

proclist_t proclist_init() {
    proclist_t list;

    list.head = list.tail = NULL;
    list.size = 0;

    return list;
}

void proclist_destroy(proclist_t* list) {
    procnode_t  
        *current = list->head,
        *del = list->head;

    while (current != NULL) {
        current = current->next;
        del = current;
        freenode(del);
    }

    freenode(del);
}

procinfo_t* proclist_append(proclist_t* list, procinfo_t info) {
    if (list->head == NULL) {   // size of 0
        list->head = list->tail = makenode(NULL, info, NULL);
        list->size += 1;
        return &list->head->value;
    } else {    // size > 0
        procnode_t* newnode = makenode( list->tail, info, NULL);
        list->tail->next = newnode;
        list->tail = newnode;
        list->size += 1;
        return &list->tail->value;
    }
}

procinfo_t* proclist_prepend(proclist_t* list, procinfo_t info) {
    if (list->head == NULL) {   // size of 0
        list->head = list->tail = makenode(NULL, info, NULL);
    } else {    // size > 0
        procnode_t* newnode = makenode( NULL, info, list->head);
        list->head->prev = newnode;
        list->head = newnode;
    }

    list->size += 1;
    return &list->head->value;
}

procinfo_t* proclist_insert(proclist_t* list, procinfo_t info) {
    if (list->head == NULL) return proclist_append(list, info);
    pid_t pid = info.pid;
    procnode_t* before = list->head;

    while (before != NULL) {
        if (pid > before->value.pid) before = before->next;
        else {
            if (before != list->head) {
                before = before->prev;
                break;
            } else {
                return proclist_prepend(list, info);
            }
        }
    }

    if (before == list->tail || before == NULL) return proclist_append(list, info);

    procnode_t* after = before->next;

    procnode_t* newnode = makenode(before, info, after);

    before->next = newnode;
    after->prev = newnode;

    return &newnode->value;
}

static procnode_t* proclist_findnode(proclist_t* list, pid_t pid) {
    if (list->head == NULL) return NULL;

    procnode_t* current = list->head;

    while (current != NULL) {
        if (current->value.pid == pid) break;
        current = current->next;
    }

    return current;
}

procinfo_t* proclist_find(proclist_t* list, pid_t pid) {
    procnode_t* fn = proclist_findnode(list, pid);

    return (fn != NULL)
        ? &fn->value
        : NULL;
}

void proclist_remove(proclist_t* list, pid_t pid) {
    procnode_t* target = proclist_findnode(list, pid);
    if (target == NULL) return;

    procnode_t* before = target->prev;
    procnode_t* after  = target->next;

    if (before) before->next = after;
    if (after)  after->prev = before;

    if (target == list->head) list->head = after;
    else 
    if (target == list->tail) list->tail = before;

    return;
}

void proclist_foreach (proclist_t* list, void (*on_value)(procinfo_t*)) {
    if (list->head == NULL) return;

    procnode_t* current = list->head;

    while (current != NULL) {
        on_value(&current->value);
        current = current->next;
    }

    return;
}


// cursor operations

#define PL_CUR_IN       0x0     // cursor is in the list
#define PL_CUR_BEFORE   0x1     // cursor is before first element
#define PL_CUR_AFTER    0x2     // cursor is after last element

proclist_cur_t pl_cur_init(proclist_t* list) {
    proclist_cur_t cursor;

    cursor.list = list;
    cursor.current = NULL;
    cursor.pos = PL_CUR_BEFORE;

    return cursor;
}
proclist_cur_t pl_cur_clone(proclist_cur_t* cur) {
	proclist_cur_t cursor = *cur;

	/*
	cursor.list = cur->list;
	cursor.current = cur->current;
	cursor.pos = cur->pos;
	*/

	return cursor;
}
u8 pl_cur_hasnext(proclist_cur_t* cur) {
    if (cur->current)
        return (cur->current->next != NULL) ? 1 : 0;
    else 
        return (cur->pos == PL_CUR_BEFORE) ? 1 : 0;
}
u8 pl_cur_hasprev(proclist_cur_t* cur) {
    if (cur->current)
        return (cur->current->prev != NULL) ? 1 : 0;
    else 
        return (cur->pos == PL_CUR_AFTER) ? 1 : 0;
}
u8 pl_cur_eq(proclist_cur_t* a, proclist_cur_t* b) {
    // XXX: hmmmm
    return (a->current == b->current && a->pos == b->pos)
        ? 1 
        : 0;
}
procinfo_t* pl_cur_at(proclist_cur_t* cur) {
    return PL_CURVAL(cur);
}
procinfo_t* pl_cur_next(proclist_cur_t* cur) {
	if (cur->pos == PL_CUR_AFTER) return NULL;
    cur->current = (cur->pos == PL_CUR_BEFORE)
        ? cur->list->head
        : cur->current->next;
    cur->pos = (cur->current == NULL)
        ? PL_CUR_AFTER
        : PL_CUR_IN;
    return PL_CURVAL(cur);
}
procinfo_t* pl_cur_prev(proclist_cur_t* cur) {
	if (cur->pos == PL_CUR_BEFORE) return NULL;
	cur->current = (cur->pos == PL_CUR_AFTER)
		? cur->list->tail
		: cur->current->prev;
	cur->pos = (cur->current == NULL)
		? PL_CUR_BEFORE
		: PL_CUR_IN;
	return PL_CURVAL(cur);
}
procinfo_t* pl_cur_first(proclist_cur_t* cur) {
    cur->current = cur->list->head;
    cur->pos = PL_CUR_IN;
    return PL_CURVAL(cur);
}
procinfo_t* pl_cur_last(proclist_cur_t* cur) {
    cur->current = cur->list->tail;
    cur->pos = PL_CUR_IN;
    return PL_CURVAL(cur);
}
void pl_cur_remove(proclist_cur_t* cur, u8 shift_dir) {
    proclist_t* list = cur->list;

    procinfo_t* rm = PL_CURVAL(cur);

    if (rm == NULL) return;

    if (shift_dir == PL_CURSHIFT_LEFT) {
        pl_cur_prev(cur);
    } else {
        pl_cur_next(cur);
    }

    proclist_remove(list, rm->pid);
}
