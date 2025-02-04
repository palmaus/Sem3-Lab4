#ifndef HEAPSORT_H
#define HEAPSORT_H

#include "ISorter.h"
#include <utility>

template <typename T, typename Comparator>
class HeapSort : public ISorter<T, Comparator> {
private:
    int sequenceLength_;
    int heapSize_ = 0;
    int currentRootIndex_ = 0;
    bool isSortFinished_ = false;
    const Comparator* comparator_ = nullptr;
    std::pair<int, int> lastChangedIndices_ = std::make_pair(-1, -1);

    void heapify(SharedPtr<MutableSequence<T>> sequence, int rootIndex) {
        int largestElementIndex = rootIndex;
        int leftChildIndex = 2 * rootIndex + 1;
        int rightChildIndex = 2 * rootIndex + 2;

        if (leftChildIndex < heapSize_ && (*comparator_)(sequence->get(largestElementIndex), sequence->get(leftChildIndex))) {
            largestElementIndex = leftChildIndex;
        }

        if (rightChildIndex < heapSize_ && (*comparator_)(sequence->get(largestElementIndex), sequence->get(rightChildIndex))) {
            largestElementIndex = rightChildIndex;
        }

        if (largestElementIndex != rootIndex) {
            lastChangedIndices_ = std::make_pair(rootIndex, largestElementIndex);
            std::swap(sequence->get(rootIndex), sequence->get(largestElementIndex));
            heapify(sequence, largestElementIndex);
        }
    }

    void buildHeap(SharedPtr<MutableSequence<T>> sequence) {
        for (int i = sequenceLength_ / 2 - 1; i >= 0; i--) {
            heapify(sequence, i);
        }
    }

public:
    std::pair<int, int> getChangedIndices() override {
        return lastChangedIndices_;
    }

    void Sort(SharedPtr<MutableSequence<T>> sequence, const Comparator& comp) override {
        sequenceLength_ = sequence->getLength();
        heapSize_ = sequenceLength_;
        isSortFinished_ = false;
        currentRootIndex_ = sequenceLength_ - 1;
        comparator_ = &comp;
        sequence_ = sequence;

        if (sequenceLength_ <= 1) {
            isSortFinished_ = true;
        }

        buildHeap(sequence);
    }

    bool isFinished() override {
        return isSortFinished_;
    }

    bool step() override {
        if (isSortFinished_) return false;

        lastChangedIndices_ = std::make_pair(-1, -1);

        if (currentRootIndex_ > 0) {
            heapify(sequence_, 0);

            lastChangedIndices_ = std::make_pair(0, currentRootIndex_);
            std::swap(sequence_->get(0), sequence_->get(currentRootIndex_));

            heapSize_--;
            currentRootIndex_--;
            return true;
        } else {
            isSortFinished_ = true;
        }

        return false;
    }

private:
    SharedPtr<MutableSequence<T>> sequence_;
};

#endif // HEAPSORT_H