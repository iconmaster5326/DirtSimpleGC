/**
 * This is the C++ implementation of the Dirt Simple Garbage Collector (or DSGC).
 * This project is located at https://github.com/iconmaster5326/DirtSimpleGC, and is given under the MIT license.
 *
 * It's an enormous work in progress compared to the C version, so be forewarned.
 */

#include <iterator>

namespace dsgc {
	using namespace std;

	// Forward references to classes listed below.
	template <class T>
	class gc_manager;

	template <class T, class Manager = gc_manager<T>>
	class gc;

	/**
	 * This class is a garbage collection manager. Extend this class to add custom GC behavior.
	 * NEVER make these things yourself! Use the class gc below!
	 * Nor should you need to manipulate the ref count yourself- C++ is smart, and will manipulate it automatically.
	 */
	template <class T>
	class gc_manager {
		public:
			unsigned refs;
			T data;

			gc_manager(T t);

			/**
			 * The iterator for listing this memory block's dependents.
			 * Extend this class to implement the dependent-listing iterator for your custom type.
			 * It's the standard C++ input iterator you know and love.
			 */
			class iterator : public std::iterator<input_iterator_tag, gc<T>*> {
				public:
					virtual iterator& operator++() {return *this;}
					virtual iterator operator++(int) {iterator tmp(*this); return tmp;}
					virtual bool operator==(const iterator& rhs) const {return true;}
					virtual bool operator!=(const iterator& rhs) const {return false;}
					virtual gc<T>* operator*() {throw NULL;}
					virtual ~iterator() {}
			};

			/**
			 * This function is called when the block is about to be freed,
			 * AFTER any dependent blocks are freed but BEFORE the block itself is freed.
			 */
			virtual ~gc_manager();

			/**
			 * Begin the iterator. Override this function to make it return your iterator subclass.
			 */
			virtual iterator begin();

			/**
			 * Override this function to make it return where your iterator subclass ends.
			 */
			virtual iterator end();

			void ref();
			void deref();
	};

	/**
	 * This class actually puts something in garbage collection.
	 * To give it a custom gc_manager, specify a second template parameter.
	 */
	template <class T, class Manager>
	class gc {
		public:
			Manager* manager;

			/**
			 * Allocates a new block of memory in garbage collection.
			 *
			 */
			gc(T t);

			/**
			 * The copy constructor. Automatically increases the ref count.
			 */
			gc(const gc<T> &other);

			/**
			 * The destructor. Automatically decreases the ref count.
			 */
			virtual ~gc();

			/**
			 * Actually retrieves the contents of the garbage collected memory area.
			 */
			T& operator *();
	};

	/**
	 * Manually trigger a collection of the garbage collector, including cycle-breaking activities.
	 * Use this if you don't like DSGC's default behavior with running collections.
	 */
	void gc_collect();
}
