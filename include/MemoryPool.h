#pragma once

#include <vector>
#include <stack>
#include <stdexcept>

/*
 * MemoryPool Class
 * 
 * A templated memory pool class for preallocating and managing objects. This helps in optimizing 
 * memory allocation and ensuring memory safety by reducing the frequency of allocations and deallocations.
 */
template<typename T>
class MemoryPool {
public:
    // constructor to preallocate memory for a given number of objects
    MemoryPool(size_t preallocateSize = 1000) {
        for (size_t i = 0; i < preallocateSize; ++i) {
            pool.push(new T());
        }
    }

    // destructor to clean up allocated memory
    ~MemoryPool() {
        while (!pool.empty()) {
            delete pool.top();
            pool.pop();
        }
    }

    /*
    * Allocate an object from the pool. If no objects are available in the pool, create a new one.
    */
    T* allocate() {
        if (pool.empty()) {
            // If no available objects in pool, create a new one
            return new T();
        }
        T* obj = pool.top();
        pool.pop();
        return obj;
    }

    /*
    * Deallocate an object and return it to the pool.
    */
    void deallocate(T* obj) {
        if (obj != nullptr) {
            pool.push(obj);
        }
    }

private:
    std::stack<T*> pool; // Stack to manage the preallocated objects
};
