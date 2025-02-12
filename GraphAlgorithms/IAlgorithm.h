#ifndef IALGORITHM_H
#define IALGORITHM_H

#include "IGraph.h"
#include "SharedPtr.h"

template <typename TWeight, typename ResultType, typename TIdentifier>
class IAlgorithm {
public:
    virtual ~IAlgorithm() = default;
    virtual SharedPtr<ResultType> execute(
        const IGraph<TWeight, TIdentifier>* graph,
        IVertex<TIdentifier>* startVertex = nullptr,
        IVertex<TIdentifier>* endVertex = nullptr
    ) const = 0;
};

#endif // IALGORITHM_H