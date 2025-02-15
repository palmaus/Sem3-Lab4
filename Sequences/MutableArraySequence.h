#pragma once

#include "MutableSequence.h"
#include "DynamicArray.h"

template<class T>
class MutableArraySequence : public MutableSequence<T> {
private:
    DynamicArray<T> base;
public:
    MutableArraySequence() : base{DynamicArray<T>()} {}

    MutableArraySequence(T *items, int length) : base{DynamicArray<T>(items, length)} {}

    explicit MutableArraySequence(const DynamicArray<T> &array) : base{DynamicArray<T>(array)} {}

    MutableArraySequence(const MutableArraySequence<T>& other) : base(other.base) {}

    const T &getFirst() const override {
        return base.getByIndex(0);
    }

    const T &getLast() const override {
        return base.getByIndex(base.getSize() - 1);
    }

    const T &get(int index) const override {
        return base.getByIndex(index);
    }

    T &get(int index) {
        return base.getByIndex(index);
    }

    SharedPtr<MutableSequence<T>> getSubSequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= base.getSize() || startIndex > endIndex)
            throw std::out_of_range("IndexOutOfRange");
        int newSize = endIndex - startIndex + 1;
        DynamicArray<T> newArray(newSize);
        for (int i = startIndex; i <= endIndex; ++i) {
            newArray.set(i - startIndex, base.getByIndex(i));
        }

        return SharedPtr<MutableSequence<T>>(new MutableArraySequence(newArray));
    }

    int getLength() const override {
        return base.getSize();
    }

    void append(const T &item) override {
        base.insertAt(base.getSize(), item);
    }

    void prepend(const T &item) override {
        base.insertAt(0, item);
    }

    void set(int index, const T& value) {
        if (index < 0 || index >= getLength()) {
            throw std::out_of_range("IndexOutOfRange");
        }
        base.set(index, value);
    }

    void removeAt(int index) override {
        if (index < 0 || index >= getLength()) {
            throw std::out_of_range("IndexOutOfRange");
        }
        base.removeAt(index);
    }

    void clear() override {
        base.clear();
    }

    void resize(int newSize) {
        if (newSize < 0) {
            throw std::invalid_argument("NegativeSize");
        }
        base.setSize(newSize);
    }

    void insertAt(int index, const T &item) override {
        base.insertAt(index, item);
    }

    void concat(SharedPtr<MutableSequence<T>> sequence) override {
        int oldSize = base.getSize();
        for (int i = 0; i < sequence->getLength(); ++i) {
            base.insertAt(i + oldSize, sequence->get(i));
        }
    }

    SharedPtr<MutableSequence<T>> copy() override {
        auto result = SharedPtr<MutableSequence<T>>(new MutableArraySequence<T>(DynamicArray<T>(base)));
        return result;
    }

    Option<T> tryGetFirst() const override {
        if (getLength() == 0)
            return Option<T>();
        return Option<T>(getFirst());
    }

    Option<T> tryGetLast() const override {
        if (getLength() == 0)
            return Option<T>();
        return Option<T>(getLast());
    }

    // Константная версия tryGet
    Option<T> tryGet(int index) const override {
        if (index < 0 || index >= getLength())
            return Option<T>();
        return Option<T>(get(index));
    }
};