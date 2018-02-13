/**
 * This is the C implementation of the Dirt Simple Garbage Collector (or DSGC).
 * This project is located at https://github.com/iconmaster5326/DirtSimpleGC, and is given under the MIT license.
 */

#include "dsgc.h"

#include <stdlib.h>

typedef struct gc_list gc_list;
typedef struct gc_block gc_block;

static gc_block* heap_end = NULL;

struct gc_list {
    gc_block* block;
    gc_list* next;
};

struct gc_block {
    const gc_vtable const* vtable;
    gc_block* heap_prev;
    gc_block* heap_next;
    unsigned long refs;
    char data[];
};

void* gc_alloc(int n, const gc_vtable const* vtable) {
    gc_block* block = malloc(sizeof(gc_block)+n);
    if (!block) {
        gc_collect();

        block = malloc(sizeof(gc_block)+n);
        if (!block) {
            return NULL;
        }
    }

    block->vtable = vtable;
    block->refs = 1;
    block->heap_prev = heap_end;
    block->heap_next = NULL;

    if (heap_end) heap_end->heap_next = block;
    heap_end = block;

    return &(block->data);
}

void gc_ref(void* _block) {
    if (!_block) return;
    gc_block* block = _block - sizeof(gc_block);

    block->refs++;
}

void gc_deref(void* _block) {
    if (!_block) return;
    gc_block* block = _block - sizeof(gc_block);
    if (block->refs == 0) return;

    block->refs--;

    if (block->refs == 0) {
        if (block->vtable && block->vtable->iter_begin && block->vtable->iter_next && block->vtable->iter_done) {
            void* iter = block->vtable->iter_begin(_block);
            while (!block->vtable->iter_done(_block, iter)) {
                void* _owned = block->vtable->iter_next(_block, &iter);
                gc_deref(_owned);
            }
        }

        if (block->heap_prev) {
            block->heap_prev->heap_next = block->heap_next;
        }

        if (block->heap_next) {
            block->heap_next->heap_prev = block->heap_prev;
        } else {
            heap_end = block->heap_prev;
        }

        if (block->vtable && block->vtable->on_delete) block->vtable->on_delete(_block);

        free(block);
    }
}

static int gc_find_cycle(gc_list* seen, gc_block* block, gc_block* entry) {
    if (!entry) return 0;
    if (entry->refs != 1) return 0;
    if (seen && entry == block) return 1;

    gc_list* seen_item = seen;
    while (seen_item) {
        if (seen_item->block == entry) return 0;
        seen_item = seen_item->next;
    }

    gc_list new_seen;
    new_seen.block = entry;
    new_seen.next = seen;

    if (entry->vtable && entry->vtable->iter_begin && entry->vtable->iter_next && entry->vtable->iter_done) {
        void* iter = entry->vtable->iter_begin(&(entry->data));
        while (!entry->vtable->iter_done(&(entry->data), iter)) {
            void* _owned = entry->vtable->iter_next(&(entry->data), &iter);
            gc_block* owned = _owned - sizeof(gc_block);

            if (gc_find_cycle(&new_seen, block, owned)) return 1;
        }
    }

    return 0;
}

void gc_collect() {
    gc_block* block = heap_end;
    while (block) {
        gc_block* prev = block->heap_prev;

        if (block->refs == 1 && gc_find_cycle(NULL, block, block)) {
            gc_deref(&(block->data));
            block = heap_end;
        } else {
            block = block->heap_prev;
        }
    }
}
