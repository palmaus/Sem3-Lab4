#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdexcept>
#include <utility>
#include "SharedPtr.h"
#include "IEnumerable.h"
#include "MutableSequence.h"
#include "MutableArraySequence.h"

template <typename TKey, typename TElement, typename Hash>
class DictionaryIterator;

template <typename TKey, typename TElement, typename Hash = std::hash<TKey>>
class HashTable : public IEnumerable<std::pair<TKey, TElement>> {
private:
    struct Entry {
        TKey key;
        TElement element;
        bool occupied;
        bool wasDeleted;
        Entry() : occupied(false) {}
    };

    Entry* table_;
    int count_;
    int capacity_;
    Hash hash_;

    int hashKey(const TKey& key) const {
        return hash_(key) % capacity_;
    }

    void resizeTable(int newCapacity) {
        Entry* oldTable = table_;
        int oldCapacity = capacity_;


        capacity_ = newCapacity;
        count_ = 0;
        table_ = new Entry[capacity_];

        for (int i = 0; i < oldCapacity; ++i) {
            if (oldTable[i].occupied && !oldTable[i].wasDeleted) {
                add(oldTable[i].key, oldTable[i].element);
            }
        }
        delete[] oldTable;
    }

    int findNode(const TKey& key) const {
        int index = hashKey(key);
        int originalIndex = index;

        while (table_[index].occupied || table_[index].wasDeleted) {
            if (table_[index].occupied && table_[index].key == key) {
                return index;
            }
            index = (index + 1) % capacity_;
            if (index == originalIndex) {
                break;
            }
        }
        return -1;
    }

public:
    explicit HashTable(int initialCapacity = 25)
        : capacity_(initialCapacity), count_(0) {
        table_ = new Entry[capacity_];
    }

    // Конструктор копирования
    HashTable(const HashTable& other) : capacity_(other.capacity_), count_(other.count_), hash_(other.hash_) {
        table_ = new Entry[capacity_];
        for (int i = 0; i < capacity_; ++i) {
            table_[i] = other.table_[i];
        }
    }

    // Оператор присваивания
    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            // Освобождаем старую память
            delete[] table_;

            // Копируем данные из другого объекта
            capacity_ = other.capacity_;
            count_ = other.count_;
            hash_ = other.hash_;

            table_ = new Entry[capacity_];
            for (int i = 0; i < capacity_; ++i) {
                table_[i] = other.table_[i];
            }
        }
        return *this;
    }

    ~HashTable() {
        delete[] table_;
    }

    int getCount() const { return count_; }
    int getCapacity() const { return capacity_; }
    Entry* getTable() const { return table_; }

    TElement get(const TKey& key) const {
        int index = findNode(key);
        if (index == -1) {
            throw std::runtime_error("Element not found");
        }
        return table_[index].element;
    }

    TElement& get(const TKey& key) {
        int index = findNode(key);
        if (index == -1) {
            throw std::runtime_error("Element not found");
        }
        return table_[index].element;
    }

    void add(const TKey& key, const TElement& element) {
        if (count_ >= capacity_ * 0.75) {
            resizeTable(capacity_ * 2);
        }

        int index = hashKey(key);
        int originalIndex = index;

        while (table_[index].occupied) {
            if (table_[index].key == key) {
                table_[index].element = element;
                return;
            }
            index = (index + 1) % capacity_;
            if (index == originalIndex) {
                resizeTable(capacity_ * 2);
            }
        }

        table_[index].key = key;
        table_[index].element = element;
        table_[index].occupied = true;
        ++count_;
    }

    void remove(const TKey& key) {
        int index = findNode(key);
        if (index == -1) {
            throw std::runtime_error("Element not found");
        }

        if (table_[index].occupied) {
            table_[index].occupied = false;
            table_[index].wasDeleted = true;
            --count_;
        }
    }

    void removeAll() {
        for (int i = 0; i < capacity_; ++i) {
            if (table_[i].occupied && !table_[i].wasDeleted) {
                table_[i].occupied = false;
                table_[i].wasDeleted = true;
                --count_;
            }
        }
    }

    bool containsKey(const TKey& key) const {
        return findNode(key) != -1;
    }

    IEnumerator<std::pair<TKey, TElement>>* getEnumerator() const override {
        return new DictionaryIterator<TKey, TElement, Hash>(this, 0); // Исправляем на Hash
    }

    DictionaryIterator<TKey, TElement, Hash> begin() const {
        return DictionaryIterator<TKey, TElement, Hash>(this, 0);
    }

    DictionaryIterator<TKey, TElement, Hash> end() const {
        return DictionaryIterator<TKey, TElement, Hash>(this, capacity_);
    }

    SharedPtr<MutableSequence<std::pair<TKey, TElement>>> getAllItems() const {
        auto items = SharedPtr<MutableSequence<std::pair<TKey, TElement>>>(new MutableArraySequence<std::pair<TKey, TElement>>());
        auto it = this->begin();
        auto end = this->end();
        while (it != end) {
            items->append(*it);
            ++it;
        }
        return items;
    }
};

#endif // HASHTABLE_H