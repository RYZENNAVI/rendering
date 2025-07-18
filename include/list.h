// Copyright 2025 Prof. Dr.-Ing. Volker Roth
// See accompanying README for license information (wasn't provided?)
// Author of a rewrite: Iaroslav Tretiakov
// Day: 09.06.2025

#ifndef LIST_H
#define LIST_H

typedef struct node node;
struct node {
    node *succ;
    node *pred;
};

typedef struct d_linked_list d_linked_list;
struct d_linked_list {
    node *first;
    node *null;
    node *last;
};

void list_init(d_linked_list *list);
void list_insert_after(node *pred, node *new_node);
void list_insert_before(node *new_node, node *succ);
void list_add_head(d_linked_list *list, node *new_node);
void list_add_tail(d_linked_list *list, node *new_node);
void list_remove_node(node *target_node);
node *list_remove_head(d_linked_list *list);
node *list_remove_tail(d_linked_list *list);
void list_append(d_linked_list *destination, d_linked_list *source);
void list_append_after(node *pred, d_linked_list *source);

#endif