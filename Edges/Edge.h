#ifndef EDGE_H
#define EDGE_H

#include "IEdge.h"
#include "IVertex.h"
#include "Vertex.h"
#include "SharedPtr.h"

template <typename TWeight, typename TIdentifier>
class Edge : public IEdge<TWeight, TIdentifier> {
private:
    IVertex<TWeight, TIdentifier>* from_;
    IVertex<TWeight, TIdentifier>* to_;
    TWeight weight_;

public:
    Edge(IVertex<TWeight, TIdentifier>* from, IVertex<TWeight, TIdentifier>* to, TWeight weight)
        : from_(from), to_(to), weight_(weight) {}
    ~Edge() override = default;

    IVertex<TWeight, TIdentifier>* getFrom() const override { return from_; }
    IVertex<TWeight, TIdentifier>* getTo() const override { return to_; }
    TWeight getWeight() const override { return weight_; }
};

#endif // EDGE_H