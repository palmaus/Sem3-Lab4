#ifndef BUBBLESORT_H
#define BUBBLESORT_H

#include "ISorter.h"
#include <utility>

template <typename T, typename Comparator>
class BubbleSort : public ISorter<T, Comparator> {
private:
    int outerLoopIndex_ = 0;
    int innerLoopIndex_ = 0;
    int sequenceLength_ = 0;
    bool isSortFinished_ = false;
    const Comparator* comparator_ = nullptr;

public:

    void Sort(SharedPtr<MutableSequence<T>> sequence, const Comparator& comp) override {
        sequenceLength_ = sequence->getLength();
        isSortFinished_ = false;
        outerLoopIndex_ = 0;
        innerLoopIndex_ = 0;

        if (sequenceLength_ <= 1) {
            isSortFinished_ = true;
        }
        comparator_ = &comp;
        sequence_ = sequence;
    }

    std::pair<int, int> getChangedIndices() override {
        return (innerLoopIndex_ < sequenceLength_ - 1) ? std::make_pair(innerLoopIndex_, innerLoopIndex_ + 1) : std::make_pair(-1, -1);
    }

    bool isFinished() override {
        return isSortFinished_;
    }

    bool step() override {
        if (isSortFinished_) return false;

        if (outerLoopIndex_ < sequenceLength_ - 1) {
            if (innerLoopIndex_ < sequenceLength_ - outerLoopIndex_ - 1) {
                if ((*comparator_)(sequence_->get(innerLoopIndex_ + 1), sequence_->get(innerLoopIndex_))) {
                    std::swap(sequence_->get(innerLoopIndex_), sequence_->get(innerLoopIndex_ + 1));
                    return true;
                }
                innerLoopIndex_++;
            } else {
                outerLoopIndex_++;
                innerLoopIndex_ = 0;
            }

        } else {
            isSortFinished_ = true;
        }
        return false;
    }

private:
    SharedPtr<MutableSequence<T>> sequence_;
};


#endif // BUBBLESORT_H