# DirtSimpleGC

Garbage collectors are a difficult thing to write, especially in systems where weak pointers are impossible and references cycles very much so. Moreover, most general-purpose implementations are forced to do terrible things like walk the call stack or inspect the heap's internals. To make matters worse, garbage collectors have to be highly performant as well!

This project throws that last tenant out the window and just tries to work, because sometimes you just need a garbage collector and don't want all the fuss.

## Overview

The Dirt Simple Garbage Collector (or DSGC) is, as the name states, a maximally simple garbage-collection algorithm, suitable for rolling into just about any project. Currently, DSGC has been implemented for C and C++. They should all be very portable (although the C++ version requires at least C++11).

Garbage collection works like this in DSGC: Every block of memory tracked by the garbage collector has a _reference count_, and any number of other blocks of memory can _depend_ on it. The reference count is maintained internally by the system, while you must give DSGC a list of your dependent  memory blocks via an iterator-like interface. The reference count starts at 1 for any block, and gets incremented when used, and decremented when fallen out of use. When the reference count hits 0, the block of memory is freed and any dependent blocks have their reference count decremented automatically.

The basic operation above doesn't help break _reference cycles_, however. When memory block A is dependent on memory block B and vice versa, you have a situation where nobody is referencing A or B, yet they still seem valid. This is fixed with _cycle breaking_. Once in a while, DSGC will look through all the allocated blocks, detect cycles, and break them so the memory be freed.

## Details

DSGC isn't fast. It isn't efficent. Fast and efficent garbage collectors are big and hard to write. Put this in production code only if you want long, slow garbage collection times.

DSGC doesn't support pointer aliasing; so if you do pointer arithmetic and then attempt to update the ref count on the thing, DSGC will most likely crash. Supporting pointer aliasing would require walking the heap with every call to the garbage collector, which is an understandably expensive operation.

The necessity of a dependent-listing iterator is so DSGC can automatically break reference cycles when garbage collecting. It only attempts to break cycles as a last resort, when it's starting to run out of memory. It's a slow operation, so putting it off is generally a good idea.

DSGC is not thread-safe (yet).

## Building

Check out `src/c/README.md` and `src/c++/README.md` for details, depending on the version you want.
