#ifndef IVERTEX_H
#define IVERTEX_H

#include "MutableArraySequence.h"

template <typename TWeight, typename TIdentifier>
class IEdge;

template <typename TWeight, typename TIdentifier>
class IVertex {
public:
    virtual ~IVertex() = default;
    virtual TIdentifier getId() const = 0;
    virtual void setId(TIdentifier id) = 0;

    virtual MutableArraySequence<IEdge<TWeight, TIdentifier>*> getIncomingEdges() const = 0;
    virtual MutableArraySequence<IEdge<TWeight, TIdentifier>*> getOutgoingEdges() const = 0;
};

#endif // IVERTEX_H

