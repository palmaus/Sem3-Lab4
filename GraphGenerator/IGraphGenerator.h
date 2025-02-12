#ifndef IGRAPHGENERATOR_H
#define IGRAPHGENERATOR_H

#include "IGraph.h"
#include "Vertex.h"
#include "Edge.h"

template <typename Weight, typename TIdentifier>
class IGraphGenerator {
public:
    virtual ~IGraphGenerator() = default;

    virtual IGraph<Weight, TIdentifier>* generate() = 0;
};

#endif // IGRAPHGENERATOR_H