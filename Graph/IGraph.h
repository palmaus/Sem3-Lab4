#ifndef IGRAPH_H
#define IGRAPH_H

#include "MutableArraySequence.h"
#include "IVertex.h"
#include "IEdge.h"
#include <limits>
#include <stdexcept>

template <typename TWeight, typename TIdentifier>
class IGraph {
public:
    virtual ~IGraph() = default;

    virtual void addVertex(IVertex<TIdentifier>* vertex) = 0;
    virtual void addEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex, TWeight weight) = 0;
    virtual void removeVertex(IVertex<TIdentifier>* vertex) = 0;
    virtual void removeEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) = 0;
    virtual MutableArraySequence<IVertex<TIdentifier>*> getVertices() const = 0;
    virtual MutableArraySequence<IEdge<TWeight, TIdentifier>*> getEdges(IVertex<TIdentifier>* vertex) const = 0;
    virtual IVertex<TIdentifier>* getVertexById(TIdentifier vertexId) const = 0;
    virtual bool hasVertex(IVertex<TIdentifier>* vertex) const = 0;
    virtual bool hasEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) const = 0;
};

#endif // IGRAPH_H