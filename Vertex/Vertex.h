#ifndef VERTEX_H
#define VERTEX_H

#include "IVertex.h"
#include <string>

class Vertex : public IVertex {
private:
    size_t id_;
    std::string name_;

public:
    Vertex(size_t id) : id_(id), name_("") {}
    Vertex(size_t id, const std::string& name) : id_(id), name_(name) {}
    ~Vertex() override = default;

    size_t getId() const override { return id_; }
    void setId(size_t id) override { id_ = id; }
    std::string getName() const override { return name_; }
    void setName(const std::string& name) override { name_ = name; }
};

#endif //VERTEX_H
