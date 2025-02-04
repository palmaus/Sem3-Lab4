#ifndef IALGORITHM_H
#define IALGORITHM_H

#include "IGraph.h"
#include "SharedPtr.h"

template <typename Weight, typename ResultType>
class IAlgorithm {
public:
    virtual ~IAlgorithm() = default;
    virtual SharedPtr<ResultType> execute(
        const IGraph<Weight>* graph,
        IVertex* startVertex = nullptr,
        IVertex* endVertex = nullptr
    ) const = 0;
};

#endif // IALGORITHM_H