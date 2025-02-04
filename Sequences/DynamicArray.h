#pragma once

#include <stdexcept>

template<typename T>
class DynamicArray {
private:
    T *data;
    int allocatedMemory;
    int size;

    void resize(int capacity) {
        if (capacity < size) throw std::invalid_argument("NegativeResize");
        T *newData = new T[capacity];
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        allocatedMemory = capacity;
    }

public:
    DynamicArray() : data{new T[5]}, allocatedMemory{5}, size{0} {}

    DynamicArray(int length) : allocatedMemory{length + 5}, size{length} {
        if (length < 0) throw std::invalid_argument("NegativeLength");
        this->data = new T[length + 5];
        for (int i = 0; i < size; ++i) {
            this->data[i] = T{};
        }
    }

    DynamicArray(const T *items, int length) : allocatedMemory{length + 5}, size{length} {
        if (length < 0) throw std::invalid_argument("NegativeLength");
        this->data = new T[length + 5];
        for (int i = 0; i < length; ++i) {
            this->data[i] = items[i];
        }
    }

    DynamicArray(const DynamicArray<T> &array) : allocatedMemory{array.allocatedMemory}, size{array.size} {
        this->data = new T[array.allocatedMemory];
        for (int i = 0; i < array.size; ++i) {
            this->data[i] = array.data[i];
        }
    }

    ~DynamicArray() {
        delete[] data;
    }

    // Перегрузка оператора присваивания
    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this != &other) { // Проверка на самоприсваивание
            delete[] data;      // Освобождаем старую память
            size = other.size;
            allocatedMemory = other.allocatedMemory;
            data = new T[allocatedMemory];
            for (int i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    T &getByIndex(int index) const {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");
        return data[index];
    }

    T &getByIndex(int index) {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");
        return data[index];
    }

    

    void set(int index, const T& value) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("IndexOutOfRange");
        }
        data[index] = value;
    }

    void removeAt(int index) {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");
        for (int i = index; i < size - 1; ++i) {
            data[i] = data[i + 1];
        }
        size--;
    }

    void clear() {
        delete[] data;
        data = new T[5];
        allocatedMemory = 5;
        size = 0;
    }

    void insertAt(int index, const T &value) {
        if (index < 0 || index > size) throw std::out_of_range("IndexOutOfRange");
        if (size + 1 > allocatedMemory) {
            resize(allocatedMemory + 5);
        }
        for (int i = size; i > index; --i) {
            data[i] = data[i - 1];
        }
        data[index] = value;
        size++;
    }

    int getSize() const {
        return size;
    }

    void setSize(int newSize) {
        if (newSize < 0) {
            throw std::invalid_argument("NegativeSize");
        }
        if (newSize > allocatedMemory) {
            resize(newSize + 5);
        }
        size = newSize;
    }
};