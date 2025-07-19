module;

// just 3d array. Content invalid after allocate()
// Note: reverse order to make for x {for y {for z}} order be the fastest
// NOTE2: DO NOT USE REVERSE ORDER FOR ACCESSING VIA data() - its memory layout is different

import std;
import glm;

using namespace glm;

#include <uchar.h>
#include <stdlib.h>

export module table3d;

#define ASSERT(condition, msg)                                                                     \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::cerr << "Assertion failed: " #condition ", " msg ", in " << __FILE__ << ", line " \
                      << __LINE__ << std::endl;                                                    \
            std::abort();                                                                          \
        }                                                                                          \
    } while (false)

export template <typename Type, bool row_major = true>
class table3d {
  private:
    Type *memory = nullptr;  // pointer to the 3D array in a linear layout
    uvec3 _size = {0, 0, 0}; // size of the 3D array

  public:
    // allocates memory for the 3D array. I like it being explicit
    void allocate(int x, int y, int z) {
        ASSERT(x > 0 && y > 0 && z > 0, "dimensions must be positive");
        _size = {x, y, z};
        // memory = new Type[x * y * z](); // allocate memory AND ZERO it
        memory = (Type *) calloc(x * y * z, sizeof(Type)); // allocate memory AND ZERO it memory
    }

    int linear_index(int x, int y, int z) const {
        if constexpr (row_major) {
            // Row-major layout (x, y, z access is fastest)
            return x + _size.x * y + _size.x * _size.y * z;
        } else {
            // Note: such order to make for x {for y {for z}} order be the fastest
            // Column-major layout (z, y, x access is fastest)
            return z + _size.z * y + _size.z * _size.y * x;
        }
    }

    void allocate(uvec3 size) {
        allocate(size.x, size.y, size.z);
    }

    // free allocated memory
    void deallocate() {
        ASSERT(memory != nullptr, "memory is already deallocated");
        // delete[] memory;
        free(memory);
        memory = nullptr;
        _size = {0, 0, 0};
    }

    // set all elements to a specific value
    void set(Type val) {
        for (int x = 0; x < _size.x; x++) {
            for (int y = 0; y < _size.y; y++) {
                for (int z = 0; z < _size.z; z++) {
                    (*this)(x, y, z) = val;
                }
            }
        }
    }

    // access elements using (x, y, z) coordinates
    Type &operator()(int x, int y, int z) {
        ASSERT(x >= 0 && x < _size.x, "X coordinate out of bounds");
        ASSERT(y >= 0 && y < _size.y, "Y coordinate out of bounds");
        ASSERT(z >= 0 && z < _size.z, "Z coordinate out of bounds");
        return memory[linear_index(x, y, z)];
    }

    const Type &operator()(int x, int y, int z) const {
        ASSERT(x >= 0 && x < _size.x, "X coordinate out of bounds");
        ASSERT(y >= 0 && y < _size.y, "Y coordinate out of bounds");
        ASSERT(z >= 0 && z < _size.z, "Z coordinate out of bounds");
        return memory[linear_index(x, y, z)];
    }

    Type &operator()(uvec3 v) {
        return (*this)(v.x, v.y, v.z);
    }
    const Type &operator()(uvec3 v) const {
        return (*this)(v.x, v.y, v.z);
    }

    // access raw data pointer
    Type *data() {
        return memory;
    }
    const Type *data() const {
        return memory;
    }

    // get the size of the array
    uvec3 size() const {
        return _size;
    }

    // destructor to free memory
    ~table3d() {
        // deallocate();
    }
};