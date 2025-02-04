#pragma once

#include "MutableListSequence.h"
#include "SharedPtr.h"
#include <utility>
#include <vector>
#include "Comparators.h" //add this


template <typename T, typename Comparator>
class ISorter {
public:
    virtual void Sort(SharedPtr<MutableSequence<T>> sequence, const Comparator& comp) = 0;

    virtual std::pair<int, int> getChangedIndices() = 0;

    virtual bool isFinished() = 0;

    virtual bool step() = 0;

    virtual ~ISorter() = default;
};