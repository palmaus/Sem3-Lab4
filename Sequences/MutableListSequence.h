#pragma once

#include <memory>
#include "MutableSequence.h"
#include "LinkedList.h"

template<class T>
class MutableListSequence : public MutableSequence<T> {
private:
    LinkedList<T> base;
public:
    MutableListSequence() : base{LinkedList<T>()} {}

    MutableListSequence(T *items, int size) : base{LinkedList<T>(items, size)} {}

    explicit MutableListSequence(const LinkedList<T> &list) : base{list} {}

    const T &getFirst() const override {
        return base.getFirst();
    }

    const T &getLast() const override {
        return base.getLast();
    }

    const T &get(int index) const override {
        return base.getByIndex(index);
    }

    T &get(int index) {
        return base.getByIndex(index);
    }


    int getLength() const override {
        return base.getLength();
    }

    void append(const T &item) override {
        base.append(item);
    }

    void prepend(const T &item) override {
        base.prepend(item);
    }

    void insertAt(int index, const T &item) override {
        base.insertAt(index, item);
    }

    void removeAt(int index) override {
        base.removeAt(index);
    }

    void clear() {
        base.clear();
    }

    SharedPtr<MutableSequence<T>> getSubSequence(int startIndex, int endIndex) const override {
        SharedPtr<LinkedList<T>> subList = base.getSubList(startIndex, endIndex);
        return SharedPtr<MutableSequence<T>>(new MutableListSequence<T>(*subList));
    }

    void concat(SharedPtr<MutableSequence<T>> sequence) {
        for (int i = 0; i < sequence->getLength(); i++) {
            append(sequence->get(i));
        }
    }

    void set(int index, const T& item)
    {
        removeAt(index);
        insertAt(index, item);
    }

    SharedPtr<MutableSequence<T>> copy() {
        return SharedPtr<MutableSequence<T>>(new MutableListSequence<T>(LinkedList<T>(base)));
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

    Option<T> tryGet(int index) const override {
        if (index < 0 || index >= getLength())
            return Option<T>();
        return Option<T>(get(index));
    }
};