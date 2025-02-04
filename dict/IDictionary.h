#ifndef IDICTIONARY_H
#define IDICTIONARY_H

#include "MutableArraySequence.h"
#include "SharedPtr.h"
#include "IEnumerable.h"

template <typename TKey, typename TElement, typename Hash>
class DictionaryIterator;

template <typename TKey, typename TElement>
class IDictionary : public IEnumerable<std::pair<TKey, TElement>> {
public:
    virtual ~IDictionary() = default;
    virtual int getCount() const = 0;
    virtual int getCapacity() const = 0;
    virtual TElement get(const TKey& key) const = 0;
    virtual TElement& get(const TKey& key) = 0;
    virtual void add(const TKey& key, const TElement& element) = 0;
    virtual void remove(const TKey& key) = 0;
    virtual bool containsKey(const TKey& key) const = 0;
    virtual SharedPtr<MutableSequence<std::pair<TKey, TElement>>> getAllItems() const = 0;

    virtual IEnumerator<std::pair<TKey, TElement>>* getEnumerator() const = 0;
    virtual DictionaryIterator<TKey, TElement, std::hash<TKey>> begin() const = 0;
    virtual DictionaryIterator<TKey, TElement, std::hash<TKey>> end() const = 0;
};

#endif //IDICTIONARY_H