#pragma once
#include <stdexcept>
#include "SharedPtr.h"
#include "WeakPtr.h"


template<typename T>
class MutableSequence;

template<typename T>
class Node {
public:
    T data;
    WeakPtr<Node<T>> left;
    SharedPtr<Node<T>> right;

    explicit Node(const T &value) : data{value}, left{}, right{nullptr} {}
};


template<class T>
class LinkedList {
private:
    SharedPtr<Node<T>> first;
    SharedPtr<Node<T>> last;
    int size;

    void checkIndex(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("IndexOutOfRange");
        }
    }

    // Внутренний метод для получения первого узла
    SharedPtr<Node<T>> getFirstNodeForIterator() const {
        return first;
    }


public:

    LinkedList() : first{nullptr}, last{nullptr}, size{0} {}

    LinkedList(T *items, int size) : first{nullptr}, last{nullptr}, size{0} {
        for (int i = 0; i < size; ++i) {
            append(items[i]);
        }
    }

    LinkedList(const LinkedList<T>& list) : first{nullptr}, last{nullptr}, size{0} {
        SharedPtr<Node<T>> current = list.first;
        while (current) {
            append(current->data);
            current = current->right;
        }
    }

    explicit LinkedList(const MutableSequence<T> *sequence) : first{nullptr}, last{nullptr}, size{0} {
        for (int i = 0; i < sequence->getLength(); ++i) {
            append(sequence->get(i));
        }
    }

    void clear() {
        while (first && first->right) {
            auto next = first->right;
            first->left = WeakPtr<Node<T>>();
            first.reset();
            first = next;
        }
        first = SharedPtr<Node<T>>();
        last = SharedPtr<Node<T>>();
        size = 0;
    }

    ~LinkedList() {
        clear(); // Вызов метода очистки для удаления всех узлов
    }

    bool isEmpty() const {
        return size == 0;
    }

    T &getFirst() {
        if (!first) {
            throw std::out_of_range("ListIsEmpty");
        }
        return first->data;
    }

    T &getLast() {
        if (!last) {
            throw std::out_of_range("ListIsEmpty");
        }
        return last->data;
    }

    T &getByIndex(int index) {
        checkIndex(index);
        SharedPtr<Node<T>> temp = first;
        for (int i = 0; i < index; ++i) {
            temp = temp->right;
        }
        return temp->data;
    }

    const T &getFirst() const {
        if (!first) {
            throw std::out_of_range("ListIsEmpty");
        }
        return first->data;
    }

    const T &getLast() const {
        if (!last) {
            throw std::out_of_range("ListIsEmpty");
        }
        return last->data;
    }

    const T &getByIndex(int index) const {
        checkIndex(index);
        SharedPtr<Node<T>> temp = first;
        for (int i = 0; i < index; ++i) {
            temp = temp->right;
        }
        return temp->data;
    }

    SharedPtr<LinkedList<T>> getSubList(int startIndex, int endIndex) const {
        checkIndex(startIndex);
        checkIndex(endIndex);
        if (startIndex > endIndex) {
            throw std::invalid_argument("startIndex cannot be greater than endIndex");
        }
        auto subList = MakeShared<LinkedList<T>>();
        SharedPtr<Node<T>> current = first;
        for (int i = 0; i < startIndex; ++i) {
            current = current->right;
        }
        for (int i = startIndex; i <= endIndex; ++i) {
            subList->append(current->data);
            current = current->right;

        }
        current = SharedPtr<Node<T>>();
        return subList;
    }

    int getLength() const {
        return size;
    }

    void append(const T &value) {
        auto newNode = MakeShared<Node<T>>(value);
        if (isEmpty()) {
            first = newNode;
            last = newNode;
        } else {
            last->right = newNode;
            newNode->left = WeakPtr<Node<T>>(last);  // !!!  Используем конструктор WeakPtr
            last = newNode;
        }
        newNode = SharedPtr<Node<T>>();
        size++;
    }

    void prepend(const T &value) {
        auto newNode = MakeShared<Node<T>>(value);
        if (!first) {
            first = newNode;
            last = newNode;
        } else {
            first->left = WeakPtr<Node<T>>(newNode);
            newNode->right = first;
            first = newNode;
        }
        size++;
    }

    void insertAt(int index, const T &value) {
        if (index == 0) {
            prepend(value);
            return;
        }
        if (index == size) {
            append(value);
            return;
        }

        Node<T>* temp = first.get();
        for (int i = 0; i < index - 1; ++i) {
            temp = temp->right.get();
        }
        auto newNode = MakeShared<Node<T>>(value);
        newNode->right = temp->right;
        newNode->left = WeakPtr<Node<T>>(); // !!! Исправлено:  обнуляем WeakPt
        if (temp->right.get()) {
            temp->right.get()->left = WeakPtr<Node<T>>(newNode);
        }
        temp->right = newNode;
        size++;

    }

    void removeAt(int index) {
        checkIndex(index);

        // Удаление первого элемента
        if (index == 0) {
            auto temp = first;
            first = first->right;
            if (first) {
                first->left = WeakPtr<Node<T>>();
            } else {
                last.reset();
            }
            temp.reset();
            size--;
            return;
        }

        // Удаление последнего элемента
        if (index == size - 1) {
            auto temp = last;
            last = last->left.lock();

            if (last) {
                last->right.reset();
            } else {
                first.reset();
            }

            temp.reset();
            size--;
            return;
        }

        SharedPtr<Node<T>> current = first;
        for (int i = 0; i < index; ++i) {
            current = current->right;
        }

        if(current->left.lock()) {
            current->left.lock()->right = current->right;
        }
        if (current->right) {
            current->right->left = current->left;
        }
        size--;
    }



    LinkedList<T>* concatenate(const LinkedList<T>& other) {
        if (this->isEmpty()) {
            return new LinkedList<T>(other);
        }

        if (!other.isEmpty()) {
            SharedPtr<Node<T>> otherFirst = other.first;
            last->right = otherFirst;
            otherFirst->left = WeakPtr<Node<T>>(last);
            size += other.getLength();
            last = other.last;
        }
        return new LinkedList<T>(*this);
    }

    LinkedList<T>& operator=(const LinkedList<T>& list) {
        if (this != &list) {
            clear();

            Node<T>* temp = list.first.get();
            while (temp) {
                append(temp->data);
                temp = temp->right.get();
            }
        }
        return *this;
    }
};