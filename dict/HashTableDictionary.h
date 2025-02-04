#ifndef HASHTABLEDICTIONARY_H
#define HASHTABLEDICTIONARY_H

#include "IDictionary.h"
#include "HashTable.h"
#include "DictionaryIterator.h"
#include <utility>
#include "SharedPtr.h"

template <typename TKey, typename TElement>
class HashTableDictionary : public IDictionary<TKey, TElement> {
private:
    HashTable<TKey, TElement> hashTable_;

public:
    explicit HashTableDictionary(int initialCapacity = 25) : hashTable_(initialCapacity) {}

    int getCount() const override { return hashTable_.getCount(); }
    int getCapacity() const override { return hashTable_.getCapacity(); }

    TElement get(const TKey& key) const override { return hashTable_.get(key); }
    TElement& get(const TKey& key) override { return hashTable_.get(key); }

    void add(const TKey& key, const TElement& element) override {
        hashTable_.add(key, element);
    }

    void remove(const TKey& key) override {
        hashTable_.remove(key);
    }

    bool containsKey(const TKey& key) const override {
        return hashTable_.containsKey(key);
    }

    SharedPtr<MutableSequence<std::pair<TKey, TElement>>> getAllItems() const override {
        return hashTable_.getAllItems();
    }

    IEnumerator<std::pair<TKey, TElement>>* getEnumerator() const override {
        return hashTable_.getEnumerator();
    }

    DictionaryIterator<TKey, TElement> begin() const {
        return hashTable_.begin();
    }

    DictionaryIterator<TKey, TElement> end() const {
        return hashTable_.end();
    }

    // Добавлен конструктор копирования
    HashTableDictionary(const HashTableDictionary& other) : hashTable_(other.hashTable_) {}
};

#endif // HASHTABLEDICTIONARY_H