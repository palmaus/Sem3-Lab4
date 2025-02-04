#pragma once

#include "IEdge.h"
#include "IVertex.h"
#include <memory>

template <typename Weight>
class Edge : public IEdge<Weight> {
private:
    IVertex* fromVertex_;
    IVertex* toVertex_;
    Weight weight_;

public:
    Edge(IVertex* fromVertex, IVertex* toVertex, Weight weight)
        : fromVertex_(fromVertex), toVertex_(toVertex), weight_(weight) {}
    ~Edge() override = default;

    IVertex* getFrom() const override { return fromVertex_; }
    IVertex* getTo() const override { return toVertex_; }
    Weight getWeight() const override { return weight_; }
    void setWeight(Weight weight) override { weight_ = weight; }

    // Конструктор копирования
    Edge(const Edge& other) : fromVertex_(other.fromVertex_), toVertex_(other.toVertex_), weight_(other.weight_) {}

    // Оператор присваивания
    Edge& operator=(const Edge& other) {
        if (this != &other) {
            fromVertex_ = other.fromVertex_;
            toVertex_ = other.toVertex_;
            weight_ = other.weight_;
        }
        return *this;
    }
};