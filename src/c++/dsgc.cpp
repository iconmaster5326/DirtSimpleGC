/**
 * This is the C++ implementation of the Dirt Simple Garbage Collector (or DSGC).
 * This project is located at https://github.com/iconmaster5326/DirtSimpleGC, and is given under the MIT license.
 *
 * It's an enormous work in progress compared to the C version, so be forewarned.
 */

#include "dsgc.hpp"

#include <list>
#include <unordered_set>

using namespace std;
using namespace dsgc;

static list<gc_manager<void*>*> gc_heap;

template <class T> dsgc::gc_manager<T>::gc_manager(T t) {
    refs = 1;
    data = t;
}

template <class T> dsgc::gc_manager<T>::~gc_manager() {

}

template <class T> typename dsgc::gc_manager<T>::iterator gc_manager<T>::begin() {
    return gc_manager<T>::iterator();
}
template <class T> typename dsgc::gc_manager<T>::iterator gc_manager<T>::end() {
    return gc_manager<T>::iterator();
}

template <class T> void  dsgc::gc_manager<T>::ref() {
    refs++;
}

template <class T> void dsgc::gc_manager<T>::deref() {
    if (refs == 0) return;
    refs--;

    if (refs == 0) {
        for (auto item = begin(); item != end(); ++item) {
            (*item)->manager->deref();
        }

        gc_heap.remove((gc_manager<void*>*) this);

        delete this;
    }
}

template <class T, class Manager> dsgc::gc<T, Manager>::gc(T t) {
    try {
        manager = new Manager(t);
    } catch (exception e) {
        gc_collect();
        manager = new Manager(t);
    }

    gc_heap.push_back((gc_manager<void*>*) manager);
}

template <class T, class Manager> dsgc::gc<T, Manager>::gc(const gc<T> &other) {
    manager = other.manager;
    manager->ref();
}

template <class T, class Manager> dsgc::gc<T, Manager>::~gc() {
    manager->deref();
}

template <class T, class Manager> T& dsgc::gc<T, Manager>::operator *() {
    return manager->data;
}

static bool gc_find_cycle(unordered_set<gc_manager<void*>*>& seen, gc_manager<void*>* block, gc_manager<void*>* entry) {
    if (!entry) return false;
    if (entry->refs != 1) return false;
    if (seen.find(entry) == seen.end()) return false;
    if (!seen.empty() && block == entry) return true;

    seen.insert(entry);

    for (auto iter = entry->begin(); iter != entry->end(); iter++) {
        gc_manager<void*>* owned = (*iter)->manager;

        if (gc_find_cycle(seen, block, owned)) return true;
    }

    return false;
}

void dsgc::gc_collect() {
    auto iter = gc_heap.begin();
    while (iter != gc_heap.end()) {
        unordered_set<gc_manager<void*>*> seen;
        gc_manager<void*>* block = *iter;
        if (block->refs == 1 && gc_find_cycle(seen, block, block)) {
            block->deref();
            iter = gc_heap.begin();
        } else {
            iter++;
        }
    }
}
