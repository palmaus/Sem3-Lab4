#pragma once

template <typename T, typename Deleter>
class SharedPtr;

template <typename T, typename Deleter>
class WeakPtr {

private:
    friend class SharedPtr<T, Deleter>;
    typename SharedPtr<T, Deleter>::ControlBlock* controlBlock;

public:
    WeakPtr() : controlBlock(nullptr) {}

    explicit WeakPtr(const SharedPtr<T, Deleter>& shared) : controlBlock(shared.controlBlock) {
        if (controlBlock) {
            ++controlBlock->weak_count;
        }
    }

    WeakPtr(const WeakPtr& other) : controlBlock(other.controlBlock) {
        if (controlBlock) {
            ++controlBlock->weak_count;
        }
    }

    ~WeakPtr() {
        if (controlBlock) {
            if (controlBlock->weak_count > 0) {
                --controlBlock->weak_count;
            }
            if (controlBlock->weak_count == 0 && controlBlock->count == 0) {
                delete controlBlock;
                controlBlock = nullptr;
            }
        }
    }


    // Конструктор перемещения
    WeakPtr(WeakPtr&& other) noexcept : controlBlock(other.controlBlock) {
        other.controlBlock = nullptr;
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            if (controlBlock) {
                if (controlBlock->weak_count != 0) {
                    ++controlBlock->weak_count;                }

                if (controlBlock->weak_count == 0 && controlBlock->count == 0) {
                    delete controlBlock;
                }
            }
            controlBlock = other.controlBlock;
            if (controlBlock) {
                ++controlBlock->weak_count;
            }
            controlBlock = nullptr;
        }
        return *this;
    }

    // Оператор присваивания перемещением
    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;
        }
        return *this;
    }

    bool expired() const {
        return controlBlock == nullptr || controlBlock->count == 0;
    }

    explicit operator bool() const { return !expired(); }

    WeakPtr& operator=(std::nullptr_t) {
        controlBlock = nullptr;
        return *this;
    }

    std::size_t use_count() const {
        return controlBlock ? controlBlock->count : 0;
    }

    SharedPtr<T, Deleter> lock() const {
        if (this->expired()) {
            throw std::runtime_error("Trying to lock an expired WeakPtr");
        }
        return SharedPtr<T, Deleter>(*this, true);
    }


    void swap(WeakPtr& other) noexcept {
        std::swap(controlBlock, other.controlBlock);
    }

    T* getPtr() const {
        if (controlBlock) {
            return controlBlock->ptr;
        } else {
            return nullptr;
        }
    }
};