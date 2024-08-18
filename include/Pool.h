#pragma once
#include <vector>

/*
* HAVENT USED THIS YET - DIDNT NEED IT
* A pool, that can be used to allocate and free objects.
* The pool is a vector of objects, and a vector of free indices.
* The free indices are used to keep track of which objects are free.
* The pool is templated on the type of object to allocate, the type of the index,
* and the size hint.
*/
template <class T, typename ptr_t, size_t reservedMemory>
class Pool
{
public:

	// define the type of the index
	using size_t__ = typename std::underlying_type<ptr_t>::type;

	// vectors to store the allocated objects and the free indices
	std::vector<T> m_allocated;
	std::vector<ptr_t> m_free;

	/*
	* Reserves memory for the pool.
	*/
	pool() { 
		m_allocated.reserve(reservedMemory); 
	}

	/*
	* Reserves memory for the pool given a reserve size.
	*/
	pool(size_t reserve_size) { 
		m_allocated.reserve(reserve_size); 
	}

	/*
	* Returns the ptr to an object given an index using the 'get' function.
	*/
	T *get(ptr_t idx) { 
		return &m_allocated[size_t__(idx)]; 
	}

	/*
	* Returns the object given an index using indexing '[]'.
	*/
	T &operator[](ptr_t idx) { 
		return m_allocated[size_t__(idx)];
	}

	/*
	* Allocates an object.
	*/
	ptr_t alloc(void) {
		if (m_free.empty()) {
			auto ret = __ptr(m_allocated.size());
			m_allocated.push_back(T());
			return ret;
		} else {
			auto ret = __ptr(m_free.back());
			m_free.pop_back();
			return ret;
		}
	}

	/*
	* Frees an object.
	*/
	void free(ptr_t idx) { 
		m_free.push_back(idx); 
	}

};
