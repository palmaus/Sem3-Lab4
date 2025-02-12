#ifndef IEDGE_H
#define IEDGE_H

#include "IVertex.h"

template <typename TWeight, typename TIdentifier>
class IEdge {
public:
    virtual ~IEdge() = default;
    virtual IVertex<TIdentifier>* getFrom() const = 0;
    virtual IVertex<TIdentifier>* getTo() const = 0;
    virtual TWeight getWeight() const = 0;
};

#endif //IEDGE_H