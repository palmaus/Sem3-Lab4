#ifndef IGRAPH_H
#define IGRAPH_H

#include "MutableArraySequence.h"
#include "IVertex.h"
#include "IEdge.h"
#include <limits>
#include <stdexcept>

template <typename Weight>
class IGraph {
public:
    virtual ~IGraph() = default;

    virtual void addVertex(IVertex* vertex) = 0;
    virtual void addEdge(IVertex* fromVertex, IVertex* toVertex, Weight weight) = 0;
    virtual void removeVertex(IVertex* vertex) = 0;
    virtual void removeEdge(IVertex* fromVertex, IVertex* toVertex) = 0;
    virtual MutableArraySequence<IVertex*> getVertices() const = 0;
    virtual MutableArraySequence<IEdge<Weight>*> getEdges(IVertex* vertex) const = 0;
    virtual IVertex* getVertexById(size_t vertexId) const = 0;
    virtual bool hasVertex(IVertex* vertex) const = 0;
    virtual bool hasEdge(IVertex* fromVertex, IVertex* toVertex) const = 0;
};

#endif // IGRAPH_H