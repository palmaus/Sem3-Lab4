#ifndef GRAPHPATH_H
#define GRAPHPATH_H

#include "IGraph.h"
#include "IVertex.h"
#include "IEdge.h"
#include "MutableArraySequence.h"
#include <stdexcept>

template <typename Weight, typename TIdentifier>
class GraphPath {
public:
    using VertexPtr = IVertex<Weight, TIdentifier>*;
    using EdgePtr = IEdge<Weight, TIdentifier>*;
    using VertexSequence = MutableArraySequence<VertexPtr>;

private:
    VertexSequence vertices_;

public:
    GraphPath() = default;
    explicit GraphPath(const VertexSequence& vertices) : vertices_(vertices) {}
    ~GraphPath() = default;

    VertexPtr getStartVertex() const {
        if (vertices_.getLength() > 0) {
            return vertices_.get(0);
        }
        return nullptr;
    }

    VertexPtr getEndVertex() const {
        if (vertices_.getLength() > 0) {
            return vertices_.get(vertices_.getLength() - 1);
        }
        return nullptr;
    }

      bool containsVertex(const VertexPtr vertex) const {
        for (size_t i = 0; i < vertices_.getLength(); ++i) {
            if (vertices_.get(i)->getId() == vertex->getId()) {
                return true;
            }
        }
        return false;
    }


    bool containsEdge(const IGraph<Weight, TIdentifier>* graph, const EdgePtr edge) const {
        for (size_t i = 0; i < vertices_.getLength() - 1; ++i) {
            VertexPtr fromVertex = vertices_.get(i);
            VertexPtr toVertex = vertices_.get(i + 1);
             if(edge->getFrom()->getId() == fromVertex->getId() && edge->getTo()->getId() == toVertex->getId())
            {
                return true;
            }
        }
        return false;
    }

    bool canConcatenate(const GraphPath<Weight, TIdentifier>& otherPath) const {
        if (getEndVertex() == nullptr || otherPath.getStartVertex() == nullptr) {
            return false;
        }
        return getEndVertex()->getId() == otherPath.getStartVertex()->getId();
    }

    GraphPath<Weight, TIdentifier> concatenate(const GraphPath<Weight, TIdentifier>& otherPath) const {
        if (!canConcatenate(otherPath)) {
            throw std::invalid_argument("Paths cannot be concatenated.");
        }

        VertexSequence newVertices;
        for (size_t i = 0; i < vertices_.getLength(); ++i) {
            newVertices.append(vertices_.get(i));
        }
        // Добавляем вершины из второго пути (кроме первой, чтобы избежать дублирования)
        for (size_t i = 1; i < otherPath.vertices_.getLength(); ++i) {
            newVertices.append(otherPath.vertices_.get(i));
        }

        return GraphPath<Weight, TIdentifier>(newVertices);
    }

    size_t getLength() const {
        return vertices_.getLength();
    }

    const VertexSequence& getVertices() const {
        return vertices_;
    }
};

#endif // GRAPHPATH_H