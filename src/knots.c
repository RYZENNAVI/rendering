// Copyright 2025 Prof. Dr.-Ing. Volker Roth
// See accompanying README for license information
// Rewrite author: Iaroslav Tretiakov & Daniel Schmidt
// Date: 10.06.2025

#include "../inc/knots.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define KNOTS_INCREMENT 1024

static d_linked_list unused_knots;

// === Tracking allocated memory blocks ===
static void **knots_blocks = NULL;
static size_t knots_block_count = 0;

void knots_init(void) {
    list_init(&unused_knots);
}

knot_t *knots_get(void) {
    uint32_t index;
    knot_t *result;

    result = (knot_t *) list_remove_head(&unused_knots);

    if (NULL == result) {
        result = calloc(KNOTS_INCREMENT, sizeof(knot_t));
        if (NULL == result) {
            fprintf(stderr, "Out of memory for knots!\n");
            exit(-1);
        }

        // Track this allocation
        void **new_blocks = realloc(knots_blocks, (knots_block_count + 1) * sizeof(void *));
        if (new_blocks == NULL) {
            fprintf(stderr, "Out of memory for block tracking!\n");
            free(result); // Avoid leak on realloc failure
            exit(-1);
        }
        knots_blocks = new_blocks;
        knots_blocks[knots_block_count++] = result;

        for (index = 0; index < KNOTS_INCREMENT; index++) {
            list_add_tail(&unused_knots, &result[index].list_node);
        }
    }

    return (knot_t *) list_remove_head(&unused_knots);
}

void knots_free(knot_t *knot) {
    list_add_tail(&unused_knots, &knot->list_node);
}

void knots_cleanup(void) {
    for (size_t i = 0; i < knots_block_count; i++) {
        free(knots_blocks[i]);
    }
    free(knots_blocks);
    knots_blocks = NULL;
    knots_block_count = 0;
}

// --------------------------------------------------------

void knots_copy(knot_t *dst, knot_t *src) {
    dst->x = src->x;
    dst->y = src->y;

    dst->left_type = src->left_type;
    dst->right_type = src->right_type;

    dst->left_info = src->left_info;
    dst->right_info = src->right_info;
}

knot_t *knots_split(knot_t *target_knot) {
    knot_t *new_knot;

    new_knot = knots_get();
    list_insert_before(&new_knot->list_node, &target_knot->list_node);

    new_knot->x = target_knot->x;
    new_knot->y = target_knot->y;

    new_knot->left_type = target_knot->left_type;
    new_knot->left_info = target_knot->left_info;

    target_knot->left_type = kt_open;
    new_knot->right_type = kt_open;

    target_knot->left_info = new_knot->left_info;
    new_knot->right_info = target_knot->right_info;
    return new_knot;
}

void knots_ring_free(knot_t *knot) {
    node *k;

    k = knot->list_node.succ;

    while (&knot->list_node != k) {
        list_remove_node(k);
        list_add_head(&unused_knots, k);
        k = knot->list_node.succ;
    }
    list_add_tail(&unused_knots, &knot->list_node);
}

knot_t *knots_ring_clone(knot_t *knots) {
    knot_t *og_node;
    knot_t *copy_node;
    knot_t *copy_node_succ;

    og_node = knots;
    copy_node = knots_get();

    copy_node->list_node.succ = &copy_node->list_node;
    copy_node->list_node.pred = &copy_node->list_node;

    knots_copy(copy_node, og_node);

    og_node = (knot_t *) og_node->list_node.succ;

    while (og_node != knots) {
        copy_node_succ = knots_get();
        knots_copy(copy_node_succ, og_node);
        list_insert_after(&copy_node->list_node, &copy_node_succ->list_node);
        copy_node = copy_node_succ;
        og_node = (knot_t *) og_node->list_node.succ;
    }
    return (knot_t *) copy_node->list_node.succ;
}

knot_t *knots_ring_reverse(knot_t *knots) {
    knot_info swap_info;
    knot_type swap_type;
    node *current_knot;
    node *right_knot;
    knot_t *left_knot;
    knot_t *new_start;

    current_knot = &knots->list_node;
    new_start = NULL;

    do {
        left_knot = (knot_t *) current_knot;

        if (kt_regular == left_knot->right_type) {
            new_start = left_knot;
        }
        swap_info = left_knot->right_info;
        left_knot->right_info = left_knot->left_info;
        left_knot->left_info = swap_info;

        swap_type = left_knot->right_type;
        left_knot->right_type = left_knot->left_type;
        left_knot->left_type = swap_type;

        right_knot = current_knot->succ;
        current_knot->succ = current_knot->pred;
        current_knot->pred = right_knot;
        current_knot = right_knot;
    } while (current_knot != &knots->list_node);

    if (NULL != new_start) {
        left_knot = (knot_t *) new_start->list_node.pred;

        new_start->left_type = kt_open;
        left_knot->right_type = kt_regular;
        return new_start;
    }
    return knots;
}
