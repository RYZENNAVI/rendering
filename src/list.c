// Copyright 2025 Prof. Dr.-Ing. Volker Roth
// See accompanying README for license information (wasn't provided?)
// Author of a rewrite: Iaroslav Tretiakov
// Day: 09.06.2025

// TODO:
//  - Add side effects to postconditions

#include "list.h"
#include <stdlib.h>

#define LINK(pred, new_node, succ)                                           \
    {                                                                        \
        new_node->succ = succ;                                               \
        new_node->pred = pred;                                               \
        succ->pred = new_node;                                               \
        pred->succ = new_node;                                               \
    }

void list_init(d_linked_list *list) {
    list->first = (node *) (&list->null);
    list->null = NULL;
    list->last = (node *) list;
}

void list_insert_after(node *pred, node *new_node) {
    node *succ;

    succ = pred->succ;
    LINK(pred, new_node, succ);
}

void list_insert_before(node *new_node, node *succ) {
    node *pred;

    pred = succ->pred;
    LINK(pred, new_node, succ);
}

void list_add_head(d_linked_list *list, node *new_node) {
    node *pred;
    node *succ;

    pred = (node *) (&list->first);
    succ = pred->succ;
    LINK(pred, new_node, succ);
}

void list_add_tail(d_linked_list *list, node *new_node) {
    node *pred;
    node *succ;

    succ = (node *) (&list->null);
    pred = succ->pred;
    LINK(pred, new_node, succ);
}

void list_remove_node(node *target_node) {
    node *pred;
    node *succ;

    pred = target_node->pred;
    succ = target_node->succ;
    pred->succ = succ;
    succ->pred = pred;
}

//? Maybe rewrite so it can also delete lists?
node *list_remove_head(d_linked_list *list) {
    node *head;

    head = list->first;
    if (head->succ != NULL) {
        list_remove_node(head);
        return head;
    }
    return NULL;
}

//? Maybe rewrite so it can also delete lists?
node *list_remove_tail(d_linked_list *list) {
    node *tail;

    tail = list->last;
    if (tail->pred != NULL) {
        list_remove_node(tail);
        return tail;
    }
    return NULL;
}

void list_append(d_linked_list *destination, d_linked_list *source) {
    if (source->first->succ == NULL) {
        return;
    }
    source->last->succ = destination->last->succ;
    destination->last->succ = source->first;
    source->first->pred = destination->last;
    destination->last = source->last;

    source->first = (node *) (&source->null);
    source->last = (node *) source;
}

void list_append_after(node *pred, d_linked_list *source) {
    if (source->first->succ == NULL) {
        return;
    }
    source->last->succ = pred->succ;
    pred->succ->pred = source->last;
    source->first->pred = pred;
    pred->succ = source->first;

    source->first = (node *) (&source->null);
    source->last = (node *) source;
}
