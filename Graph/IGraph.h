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

    virtual void addVertex(IVertex<TWeight, TIdentifier>* vertex) = 0;
    virtual void addEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex, TWeight weight) = 0;
    virtual void removeVertex(IVertex<TWeight, TIdentifier>* vertex) = 0;
    virtual void removeEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) = 0;
    virtual MutableArraySequence<IVertex<TWeight, TIdentifier>*> getVertices() const = 0;
    virtual MutableArraySequence<IEdge<TWeight, TIdentifier>*> getEdges(IVertex<TWeight, TIdentifier>* vertex) const = 0;
    virtual IVertex<TWeight, TIdentifier>* getVertexById(TIdentifier vertexId) const = 0;
    virtual bool hasVertex(IVertex<TWeight, TIdentifier>* vertex) const = 0;
    virtual bool hasEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) const = 0;
};

#endif // IGRAPH_H