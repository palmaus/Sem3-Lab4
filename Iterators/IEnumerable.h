#pragma once

#include "IEnumerator.h"

template <typename T>
class IEnumerable {
public:
    virtual IEnumerator<T>* getEnumerator() const = 0;
    virtual ~IEnumerable() = default;
};