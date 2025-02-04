#ifndef DICTIONARYITERATOR_H
#define DICTIONARYITERATOR_H

#include "IEnumerator.h"
#include "IDictionary.h"

template <typename TKey, typename TElement, typename Hash>
class HashTable;

template <typename TKey, typename TElement, typename Hash = std::hash<TKey>>
class DictionaryIterator : public IEnumerator<std::pair<TKey, TElement>> {
private:
    const HashTable<TKey, TElement, Hash>* hashTable_;
    int currentIndex_;

    void moveToNextValid() {
        while (currentIndex_ < hashTable_->getCapacity() &&
               !hashTable_->getTable()[currentIndex_].occupied) {
            ++currentIndex_;
               }
    }

public:
    DictionaryIterator(const HashTable<TKey, TElement, Hash>* ht, int start)
        : hashTable_(ht), currentIndex_(start) {
        moveToNextValid();
    }

    bool moveNext() override {
        ++currentIndex_;
        moveToNextValid();
        return currentIndex_ < hashTable_->getCapacity();
    }

    std::pair<TKey, TElement> current() const override {
        if (currentIndex_ >= hashTable_->getCapacity()) {
            throw std::runtime_error("Iterator out of bounds");
        }
        return {hashTable_->getTable()[currentIndex_].key, hashTable_->getTable()[currentIndex_].element};
    }

    void reset() override {
        currentIndex_ = 0;
        moveToNextValid();
    }

    bool operator==(const DictionaryIterator& other) const {
        return hashTable_ == other.hashTable_ && currentIndex_ == other.currentIndex_;
    }

    bool operator!=(const DictionaryIterator& other) const {
        return !(*this == other);
    }

    DictionaryIterator& operator++() {
        moveNext();
        return *this;
    }

    // Разыменование
    std::pair<TKey, TElement> operator*() const {
        return current();
    }
};

#endif //DICTIONARYITERATOR_H