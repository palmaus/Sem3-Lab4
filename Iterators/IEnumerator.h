#pragma once

template <typename T>
class IEnumerator {
public:
    virtual bool moveNext() = 0;
    virtual T current() const = 0;
    virtual void reset() = 0;
    virtual ~IEnumerator() = default;
};