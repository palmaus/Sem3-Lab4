#ifndef UNIQUEPTR_H
#define UNIQUEPTR_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <type_traits>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    using element_type = typename std::conditional<std::is_array<T>::value,
                         typename std::remove_extent<T>::type, T>::type;
    using pointer = T*;

private:
    pointer ptr;
    Deleter deleter;

public:
    UniquePtr() : ptr(nullptr) {}

    explicit UniquePtr(pointer p, Deleter d = Deleter()) : ptr(p), deleter(d) {}

    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    }

    ~UniquePtr() {
        deleter(ptr);
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            deleter(ptr);
            ptr = other.ptr;
            deleter = std::move(other.deleter);
            other.ptr = nullptr;
        }
        return *this;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    T& operator*() {
          if (!ptr) {
                throw std::runtime_error("Trying to dereference a null UnqPtr");
          }
          return *ptr;
    }

    const T& operator*() const {
          if (!ptr) {
                throw std::runtime_error("Trying to dereference a null UnqPtr");
          }
          return *ptr;
    }

    T* operator->() {
          if (!ptr) {
                throw std::runtime_error("Trying to dereference a null UnqPtr");
          }
          return ptr;
    }

    const T* operator->() const {
          if (!ptr) {
                throw std::runtime_error("Trying to dereference a null UnqPtr");
          }
          return ptr;
    }

    pointer get() const noexcept { return ptr; }
    Deleter& get_deleter() noexcept { return deleter; }
    const Deleter& get_deleter() const noexcept { return deleter; }

    pointer release() noexcept {
        pointer temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(pointer p = nullptr) {
        deleter(ptr);
        ptr = p;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);
    }


    friend bool operator==(const UniquePtr<T, Deleter>& lhs, const UniquePtr<T, Deleter>& rhs) { return lhs.get() == rhs.get(); }
    friend bool operator!=(const UniquePtr<T, Deleter>& lhs, const UniquePtr<T, Deleter>& rhs) { return lhs.get() != rhs.get(); }
};




template <typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, UniquePtr<T>>::type make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
typename std::enable_if<std::is_array<T>::value, UniquePtr<T>>::type make_unique(std::size_t size, Args&&... args) {
    using element_type = typename std::remove_extent<T>::type;
    return UniquePtr<T>(new element_type[size]{std::forward<Args>(args)...});
}

#endif //UNIQUEPTR_H