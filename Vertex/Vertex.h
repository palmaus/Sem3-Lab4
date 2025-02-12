#ifndef VERTEX_H
#define VERTEX_H

#include "IVertex.h"
#include <string>

template <typename TIdentifier>
class Vertex : public IVertex<TIdentifier> {
private:
    TIdentifier id_;

public:
    explicit Vertex(TIdentifier id) : id_(id) {}
    Vertex(const Vertex& other) : id_(other.id_) {} // Конструктор копирования
    ~Vertex() override = default;

    TIdentifier getId() const override { return id_; }
    void setId(TIdentifier id) override { id_ = id; }
};

#endif //VERTEX_H
