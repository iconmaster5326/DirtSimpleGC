/**
 * This is the C implementation of the Dirt Simple Garbage Collector (or DSGC).
 * This project is located at https://github.com/iconmaster5326/DirtSimpleGC, and is given under the MIT license.
 */

/**
 * A vtable, here in plain ol' C! This struct is used to store handlers for your GC'd block of memory.
 * For best results, have a global store one copy of a vtable for all objects of a given type.
 * It's okay if any of these fields are NULL; DSGC will not attempt to call them.
 */
typedef struct {
	/**
	 * This function is called when the block is about to be freed,
	 * AFTER any dependent blocks are freed but BEFORE the block itself is freed.
	 * You're given the memory block you're a part of.
	 */
    void (*on_delete)(void*);

    /**
     * Begins the dependent-listing iterator.
     * You're given the memory block you're a part of, and you return a cookie for the first call to iter_next.
     * This just sets up an iterator; you shouldn't return the first dependent memory block yet.
     */
    void* (*iter_begin)(void*);

    /**
     * Steps through the dependent-listing iterator.
     * The first argument is the memory block you're a part of.
     * The second argument is a pointer to your cookie. Mutate its contents for whatever purposes you desire.
     * Return a dependent memory block of yours here.
     * Next time this function is called, you should be returning the next one in the sequence, and so on.
     */
    void* (*iter_next)(void*,void**);

    /**
     * Determines if we're out of items in the dependent-listing iterator.
     * The first argument is the memory block you're a part of.
     * The second argument is your cookie (possibly changed previously by iter_next).
     * Return any non-zero value to indicate that the iterator is complete.
     *
     * For a zero-length iterator, return 1 when you encounter what you returned from iter_begin. Or just return 1 no matter what.
     */
    int (*iter_done)(void*,void*);
} gc_vtable;

/**
 * Allocates a garbage-collected block of memory.
 *
 * n is the number of bytes to allocate, as in malloc.
 * vtable is the gc_vtable for your block's type. May be NULL if you'd like.
 * It returns your memory block, n bytes large.
 *
 * The block given to you starts out with 1 reference on it. When this block falls out of your function's scope,
 * call gc_deref on it. If you don't, you'll leak memory.
 */
void* gc_alloc(int n, const gc_vtable const* vtable);

/**
 * Increments the reference count on a block given to you via gc_alloc. Call this when you copy the block's
 * pointer over to or from any storage location that will outlive your current scope. And be sure the thing you're putting the block into
 * uses the dependent-listing iterator, or otherwise knows about DSGC!
 */
void gc_ref(void* block);

/**
 * Decrements the reference count on a block given to you via gc_alloc. If the count is 0, the block will be free and the deref will
 * cascade to any of the block's dependents. Call this when a local variable in garbage collection is about to fall out of scope.
 */
void gc_deref(void* block);

/**
 * Manually trigger a collection of the garbage collector, including cycle-breaking activities.
 * Use this if you don't like DSGC's default behavior with running collections.
 */
void gc_collect();
