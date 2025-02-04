#ifndef MERGESORT_H
#define MERGESORT_H

#include "ISorter.h"
#include <vector>
#include <utility>
#include "MutableSequence.h"
#include "MutableArraySequence.h"

template <typename T, typename Comparator>
class MergeSort : public ISorter<T, Comparator> {
private:
    int sequenceLength_ = 0;
    bool isSortFinished_ = false;
    const Comparator* comparator_ = nullptr;
    int currentMergeSize_ = 1;
    int currentLeftStartIndex_ = 0;
    std::pair<int, int> lastChangedIndices_ = std::make_pair(-1, -1);

    void merge(SharedPtr<MutableSequence<T>> sequence, int leftStartIndex, int middleIndex, int rightEndIndex) {
        int leftSubsequenceLength = middleIndex - leftStartIndex + 1;
        int rightSubsequenceLength = rightEndIndex - middleIndex;

        auto leftSubsequence = MakeShared<MutableArraySequence<T>>();
        auto rightSubsequence = MakeShared<MutableArraySequence<T>>();

        for (int i = 0; i < leftSubsequenceLength; i++) {
            leftSubsequence->append(sequence->get(leftStartIndex + i));
        }
        for (int j = 0; j < rightSubsequenceLength; j++) {
            rightSubsequence->append(sequence->get(middleIndex + 1 + j));
        }

        int leftSubsequenceIndex = 0;
        int rightSubsequenceIndex = 0;
        int mergeIndex = leftStartIndex;
        lastChangedIndices_ = std::make_pair(-1, -1);

        while (leftSubsequenceIndex < leftSubsequenceLength && rightSubsequenceIndex < rightSubsequenceLength) {
            if ((*comparator_)(leftSubsequence->get(leftSubsequenceIndex), rightSubsequence->get(rightSubsequenceIndex))) {
                lastChangedIndices_ = std::make_pair(mergeIndex, leftStartIndex + leftSubsequenceIndex);
                sequence->set(mergeIndex, leftSubsequence->get(leftSubsequenceIndex));
                leftSubsequenceIndex++;
            } else {
                lastChangedIndices_ = std::make_pair(mergeIndex, middleIndex + 1 + rightSubsequenceIndex);
                sequence->set(mergeIndex, rightSubsequence->get(rightSubsequenceIndex));
                rightSubsequenceIndex++;
            }
            mergeIndex++;
        }

        while (leftSubsequenceIndex < leftSubsequenceLength) {
            lastChangedIndices_ = std::make_pair(mergeIndex, leftStartIndex + leftSubsequenceIndex);
            sequence->set(mergeIndex, leftSubsequence->get(leftSubsequenceIndex));
            leftSubsequenceIndex++;
            mergeIndex++;
        }

        while (rightSubsequenceIndex < rightSubsequenceLength) {
            lastChangedIndices_ = std::make_pair(mergeIndex, middleIndex + 1 + rightSubsequenceIndex);
            sequence->set(mergeIndex, rightSubsequence->get(rightSubsequenceIndex));
            rightSubsequenceIndex++;
            mergeIndex++;
        }
    }

    void mergeSortStep(SharedPtr<MutableSequence<T>> sequence) {
        if (currentLeftStartIndex_ < sequenceLength_) {
            int middleIndex = std::min(currentLeftStartIndex_ + currentMergeSize_ - 1, sequenceLength_ - 1);
            int rightEndIndex = std::min(currentLeftStartIndex_ + 2 * currentMergeSize_ - 1, sequenceLength_ - 1);
            merge(sequence, currentLeftStartIndex_, middleIndex, rightEndIndex);
            currentLeftStartIndex_ += 2 * currentMergeSize_;
        } else {
            currentMergeSize_ *= 2;
            currentLeftStartIndex_ = 0;
            if (currentMergeSize_ >= sequenceLength_)
                isSortFinished_ = true;
        }
    }

public:
    std::pair<int, int> getChangedIndices() override {
        return lastChangedIndices_;
    }

    void Sort(SharedPtr<MutableSequence<T>> sequence, const Comparator& comp) override {
        sequenceLength_ = sequence->getLength();
        isSortFinished_ = false;
        currentMergeSize_ = 1;
        currentLeftStartIndex_ = 0;
        comparator_ = &comp;
        sequence_ = sequence;
        if (sequenceLength_ <= 1) {
            isSortFinished_ = true;
        }
    }

    bool isFinished() override {
        return isSortFinished_;
    }

    bool step() override {
        if (isSortFinished_) return false;
        mergeSortStep(sequence_);
        return true;
    }

private:
    SharedPtr<MutableSequence<T>> sequence_;
};

#endif // MERGESORT_H