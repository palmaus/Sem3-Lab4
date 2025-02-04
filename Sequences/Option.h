#pragma once

template <typename T>
class Option {
private:
    bool hasValue;
    T value;

public:
    Option() : hasValue(false), value() {}
    Option(const T& value) : hasValue(true), value(value) {}

    bool isSome() const { return hasValue; }
    bool isNone() const { return !hasValue; }

    T getValue() const {
        if (!hasValue) {
            return T{};
        }
        return value;
    }

    Option(const Option&) = delete;
    Option& operator=(const Option&) = delete;
};