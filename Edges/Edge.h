#ifndef EDGE_H
#define EDGE_H

#include "IEdge.h"
#include "IVertex.h"
#include "Vertex.h"
#include "SharedPtr.h"

template <typename TWeight, typename TIdentifier>
class Edge : public IEdge<TWeight, TIdentifier> {
private:
    IVertex<TIdentifier>* from_;
    IVertex<TIdentifier>* to_;
    TWeight weight_;

public:
    Edge(IVertex<TIdentifier>* from, IVertex<TIdentifier>* to, TWeight weight)
        : from_(from), to_(to), weight_(weight) {}
    ~Edge() override = default;

    IVertex<TIdentifier>* getFrom() const override { return from_; }
    IVertex<TIdentifier>* getTo() const override { return to_; }
    TWeight getWeight() const override { return weight_; }
};

#endif // EDGE_H