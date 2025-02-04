#ifndef IVERTEX_H
#define IVERTEX_H

#include <string>
#include <memory>

class IVertex {
public:
    virtual ~IVertex() = default;
    virtual size_t getId() const = 0;
    virtual void setId(size_t id) = 0;
    virtual std::string getName() const = 0;
    virtual void setName(const std::string& name) = 0;
};

#endif //IVERTEX_H
