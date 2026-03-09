#ifndef SUPERNOVA_ARRAY_H
#define SUPERNOVA_ARRAY_H

#include <cstddef>

#ifdef SUPERNOVA_EDITOR
#include <vector>
#endif

namespace Supernova {

template <typename T, std::size_t Size>
struct HybridArray {
#ifdef SUPERNOVA_EDITOR
    std::vector<T> values;

    HybridArray() : values(Size) {}

    std::size_t size() const {
        return values.size();
    }

    bool validIndex(int index) const {
        return index >= 0;
    }

    T& operator[](std::size_t index) {
        if (index >= values.size()) {
            values.resize(index + 1);
        }
        return values[index];
    }

    const T& operator[](std::size_t index) const {
        return values[index];
    }

#else
    T values[Size]{};

    std::size_t size() const {
        return Size;
    }

    bool validIndex(int index) const {
        return index >= 0 && static_cast<std::size_t>(index) < Size;
    }

    T& operator[](std::size_t index) {
        return values[index];
    }

    const T& operator[](std::size_t index) const {
        return values[index];
    }
#endif
};

}

#endif
