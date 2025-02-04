#ifndef IEDGE_H
#define IEDGE_H

#include "IVertex.h"
#include "SharedPtr.h"

template <typename Weight>
class IEdge {
public:
    virtual ~IEdge() = default;
    virtual IVertex* getFrom() const = 0;
    virtual IVertex* getTo() const = 0;
    virtual Weight getWeight() const = 0;
    virtual void setWeight(Weight weight) = 0;
};

#endif //IEDGE_H
