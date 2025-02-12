#ifndef IVERTEX_H
#define IVERTEX_H

#include <string>
#include <memory>

template <typename TIdentifier>
class IVertex {
public:
    virtual ~IVertex() = default;
    virtual TIdentifier getId() const = 0;
    virtual void setId(TIdentifier id) = 0;
};

#endif //IVERTEX_H
