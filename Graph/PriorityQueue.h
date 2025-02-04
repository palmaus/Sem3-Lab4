#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "MutableArraySequence.h"
#include <stdexcept>
#include <algorithm>

template <typename T, typename K, typename Compare = std::less<K>>
class PriorityQueue {
private:
    struct Node {
        T item;    // Элемент очереди (например, вершина или ребро)
        K priority; // Приоритет (например, вес)

        Node() : item(T()), priority(K()) {} // Инициализация по умолчанию
        Node(const T& newItem, const K& newPriority) : item(newItem), priority(newPriority) {}
    };

    MutableArraySequence<Node> sequence;
    Compare comp;

    void heapifyUp(int index) {
        while (index > 0) {
            int parentIndex = (index - 1) / 2;
            if (!comp(sequence.get(index).priority, sequence.get(parentIndex).priority)) break;
            std::swap(sequence.get(index), sequence.get(parentIndex));
            index = parentIndex;
        }
    }

    void heapifyDown(int index) {
        int size = sequence.getLength();
        while (true) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int smallest = index;

            if (leftChild < size && comp(sequence.get(leftChild).priority, sequence.get(smallest).priority)) {
                smallest = leftChild;
            }
            if (rightChild < size && comp(sequence.get(rightChild).priority, sequence.get(smallest).priority)) {
                smallest = rightChild;
            }
            if (smallest == index) break;
            std::swap(sequence.get(index), sequence.get(smallest));
            index = smallest;
        }
    }

    [[nodiscard]] size_t parent(size_t i) const { return (i - 1) / 2; }
    [[nodiscard]] size_t leftChild(size_t i) const { return 2 * i + 1; }
    [[nodiscard]] size_t rightChild(size_t i) const { return 2 * i + 2; }

public:
    PriorityQueue() = default;

    void enqueue(const T& item, const K& priority) {
        // Проверяем, есть ли уже элемент с таким ключом в очереди
        for (int i = 0; i < sequence.getLength(); ++i) {
            if (sequence.get(i).item == item) {
                // Если есть, и новый приоритет выше, обновляем приоритет
                if (comp(priority, sequence.get(i).priority)) {
                    sequence.get(i).priority = priority;
                    heapifyUp(i);
                    heapifyDown(i);
                }
                return;
            }
        }

        // Если элемента с таким ключом нет, добавляем новый
        sequence.append(Node(item, priority));
        heapifyUp(sequence.getLength() - 1);
    }

    T dequeue() {
        if (sequence.getLength() == 0) {
            throw std::out_of_range("PriorityQueue is empty");
        }
        T first = sequence.get(0).item;
        std::swap(sequence.get(0), sequence.get(sequence.getLength() - 1));
        sequence.removeAt(sequence.getLength() - 1);
        heapifyDown(0);
        return first;
    }

    T peek() const {
        if (sequence.getLength() == 0) {
            throw std::out_of_range("PriorityQueue is empty");
        }
        return sequence.get(0).item;
    }

    bool isEmpty() const {
        return sequence.getLength() == 0;
    }

    int getLength() const {
        return sequence.getLength();
    }

    ~PriorityQueue() = default;
};

#endif // PRIORITYQUEUE_H
