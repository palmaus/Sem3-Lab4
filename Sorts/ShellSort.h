#ifndef SHELLSORT_H
#define SHELLSORT_H

#include "ISorter.h"
#include <utility>
#include <cmath>
#include <vector>

template <typename T, typename Comparator>
class ShellSort : public ISorter<T, Comparator> {
private:
    int sequenceLength_ = 0;
    std::vector<int> gapSequence_;
    int currentGapIndex_ = 0;
    int outerLoopIndex_ = 0;
    int innerLoopIndex_ = 0;
    bool isSortFinished_ = false;
    const Comparator* comparator_ = nullptr;

    void initGaps() {
        gapSequence_.clear();
        int gapSequenceIndex = 0;
        int currentGap;
        while (true) {
            if (gapSequenceIndex % 2 == 0) {
                currentGap = 9 * pow(4, gapSequenceIndex / 2) - 9 * pow(2, gapSequenceIndex / 2) + 1;
            } else {
                currentGap = 8 * pow(4, (gapSequenceIndex - 1) / 2) - 6 * pow(2, (gapSequenceIndex + 1) / 2) + 1;
            }
            if (currentGap < 1)
                break;

            gapSequence_.insert(gapSequence_.begin(), currentGap);
            gapSequenceIndex++;
        }
        currentGapIndex_ = 0;
        outerLoopIndex_ = 0;
        innerLoopIndex_ = 0;
    }

public:
    std::pair<int, int> getChangedIndices() override {
        return std::make_pair(innerLoopIndex_, innerLoopIndex_ - gapSequence_[currentGapIndex_]);
    }

    void Sort(SharedPtr<MutableSequence<T>> sequence, const Comparator& comp) override {
        sequenceLength_ = sequence->getLength();
        isSortFinished_ = false;
        comparator_ = &comp;
        sequence_ = sequence;
        initGaps();
        if (sequenceLength_ <= 1) {
            isSortFinished_ = true;
        }
    }

    bool isFinished() override {
        return isSortFinished_;
    }

    bool step() override {
        if (isSortFinished_) return false;

        if (currentGapIndex_ < gapSequence_.size()) {
            if (outerLoopIndex_ < sequenceLength_) {
                if (innerLoopIndex_ < sequenceLength_) {
                    if (innerLoopIndex_ >= gapSequence_[currentGapIndex_] && (*comparator_)(sequence_->get(innerLoopIndex_), sequence_->get(innerLoopIndex_ - gapSequence_[currentGapIndex_]))) {
                        std::swap(sequence_->get(innerLoopIndex_), sequence_->get(innerLoopIndex_ - gapSequence_[currentGapIndex_]));
                        innerLoopIndex_ -= gapSequence_[currentGapIndex_];
                        return true;
                    } else {
                        innerLoopIndex_++;
                    }
                } else {
                    outerLoopIndex_++;
                    innerLoopIndex_ = outerLoopIndex_;
                }
            } else {
                outerLoopIndex_ = 0;
                innerLoopIndex_ = 0;
                currentGapIndex_++;
                if (currentGapIndex_ >= gapSequence_.size()) {
                    if (!isSortFinished_) {
                        currentGapIndex_ = gapSequence_.size() - 1;
                        isSortFinished_ = true;
                    }

                }
            }
        }

        if (isSortFinished_ && gapSequence_.back() == 1 && outerLoopIndex_ < sequenceLength_ - 1) {
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
            return false;
        }

        return false;
    }

private:
    SharedPtr<MutableSequence<T>> sequence_;
};

#endif // SHELLSORT_H