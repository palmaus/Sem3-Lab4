#ifndef VERTEX_H
#define VERTEX_H

#include "IVertex.h"
#include "IEdge.h"

template <typename TWeight, typename TIdentifier>
class Vertex : public IVertex<TWeight, TIdentifier> {
private:
    TIdentifier id_;
    MutableArraySequence<IEdge<TWeight, TIdentifier>*> incomingEdges_;
    MutableArraySequence<IEdge<TWeight, TIdentifier>*> outgoingEdges_;

public:
    explicit Vertex(TIdentifier id) : id_(id) {}
    Vertex(const Vertex& other) : id_(other.id_), incomingEdges_(other.incomingEdges_), outgoingEdges_(other.outgoingEdges_) {}
    ~Vertex() override = default;

    TIdentifier getId() const override { return id_; }
    void setId(TIdentifier id) override { id_ = id; }


    MutableArraySequence<IEdge<TWeight, TIdentifier>*> getIncomingEdges() const override {
        return incomingEdges_;
    }

    MutableArraySequence<IEdge<TWeight, TIdentifier>*> getOutgoingEdges() const override {
        return outgoingEdges_;
    }

    void addIncomingEdge(IEdge<TWeight, TIdentifier>* edge) {
        incomingEdges_.append(edge);
    }

    void addOutgoingEdge(IEdge<TWeight, TIdentifier>* edge) {
        outgoingEdges_.append(edge);
    }


    void removeIncomingEdge(IEdge<TWeight, TIdentifier>* edge) {
        for (int i = 0; i < incomingEdges_.getLength(); i++)
        {
            if(incomingEdges_.get(i) == edge)
            {
                incomingEdges_.removeAt(i);
                return;
            }
        }
    }


    void removeOutgoingEdge(IEdge<TWeight, TIdentifier>* edge) {
        for (int i = 0; i < outgoingEdges_.getLength(); i++)
        {
            if(outgoingEdges_.get(i) == edge)
            {
                outgoingEdges_.removeAt(i);
                return;
            }
        }
    }
};

#endif