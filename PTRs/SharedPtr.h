#pragma once

#include <memory>
#include <type_traits>
#include <utility>

template <typename T, typename Deleter = std::default_delete<T>> class WeakPtr;

template <typename T, typename Deleter = std::default_delete<T>>
class SharedPtr {
public:
    struct ControlBlock {
        size_t count;
        size_t weak_count;
        T* ptr;
        Deleter deleter;

        ControlBlock(size_t cnt, size_t weak_cnt, T* p, Deleter d) : count(cnt), weak_count(weak_cnt), ptr(p), deleter(d) {}
    };

    // Конструктор по умолчанию
    SharedPtr() : controlBlock(nullptr) {}

    // Конструктор от указателя
    template <typename U = T, typename = std::enable_if_t<!std::is_array_v<U>>>
    explicit SharedPtr(T* ptr = nullptr, Deleter d = Deleter()) : controlBlock(ptr ? new ControlBlock{1, 0, ptr, d} : nullptr) {}

    // Конструктор копирования
    SharedPtr(const SharedPtr& other) : controlBlock(other.controlBlock) {
        increment();
    }

    // Конструктор перемещения
    SharedPtr(SharedPtr&& other) noexcept : controlBlock(other.controlBlock) {
        other.controlBlock = nullptr;
    }

    // Оператор присваивания копированием
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            reset();
            controlBlock = other.controlBlock;
            increment();
        }
        return *this;
    }

    template<typename U, typename D>
    bool operator!=(const SharedPtr<U, D>& other) const {
        return !(*this == other);
    }

    // Оператор присваивания перемещением
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            reset();
            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;
        }
        return *this;
    }

    SharedPtr& operator=(std::nullptr_t) {
        reset();
        return *this;
    }

    // Деструктор
    ~SharedPtr() { reset(); }

    // Оператор разыменования
    const T& operator*() const {
        if (!controlBlock || !controlBlock->ptr) {
            throw std::runtime_error("Dereferencing null SharedPtr");
        }
        return *controlBlock->ptr;
    }

    T& operator*() {
        if (!controlBlock || !controlBlock->ptr) {
            throw std::runtime_error("Dereferencing null SharedPtr");
        }
        return *controlBlock->ptr;
    }

    // Оператор стрелка
    const T* operator->() const {
        if (!controlBlock || !controlBlock->ptr) {
            throw std::runtime_error("Dereferencing null SharedPtr");
        }
        return controlBlock->ptr;
    }

    T* operator->() {
        if (!controlBlock || !controlBlock->ptr) {
            throw std::runtime_error("Dereferencing null SharedPtr");
        }
        return controlBlock->ptr;
    }

    // Получение указателя
    T* get() const { return controlBlock ? controlBlock->ptr : nullptr; }

    Deleter get_deleter() const {
        return controlBlock->deleter;
    }


    bool operator==(std::nullptr_t) const { return get() == nullptr; }
    bool operator!=(std::nullptr_t) const { return get() != nullptr; }

    template<typename U, typename D>
    bool operator==(const SharedPtr<U, D>& other) const {
        return get() == other.get();
    }


    // Получение количества ссылок
    size_t use_count() const { return controlBlock ? controlBlock->count : 0; }

    // Проверка на пустоту
    explicit operator bool() const { return controlBlock && controlBlock->ptr; }

    // Сброс указателя
    void reset() {
        if (controlBlock) {
            decrement();
        }
    }

    // Сброс с новым указателем
    template <typename U = T, typename = std::enable_if_t<!std::is_array_v<U>>>
    void reset(T* def_ptr, Deleter d = Deleter()) {
        reset();
        controlBlock = def_ptr ? new ControlBlock{1, 0, def_ptr, d} : nullptr;
    }

private:
    ControlBlock* controlBlock;

    explicit SharedPtr(const WeakPtr<T, Deleter>& weak, bool) : controlBlock(weak.controlBlock) {
        increment();
    }


    void increment() {
        if (controlBlock) {
            ++controlBlock->count;
        }
    }

    void decrement() {
        if (controlBlock && --controlBlock->count == 0) {
            if (controlBlock->ptr) {
                controlBlock->deleter(controlBlock->ptr);
                controlBlock->ptr = nullptr;
            }
            if (controlBlock->weak_count == 0) {
                delete controlBlock;
            }
        }
        controlBlock = nullptr;
    }


    friend class WeakPtr<T, Deleter>;

    template <typename U, typename D, typename... Args>
    friend SharedPtr<U, D> MakeShared(Args&&... args);

    template <typename U, typename D, typename... Args>
    friend SharedPtr<U, D> MakeSharedForArray(std::size_t size, Args&&... args);
};

template <typename T, typename Deleter = std::default_delete<T>, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...), Deleter{});
}

template <typename T, typename Deleter = std::default_delete<T>, typename... Args>
SharedPtr<T> MakeSharedForArray(std::size_t size, Args&&... args) {
    return SharedPtr<T>(new T[size](std::forward<Args>(args)...), Deleter{});
}