#ifndef GRAPHPATH_H
#define GRAPHPATH_H

#include "IGraph.h"
#include "IVertex.h"
#include "IEdge.h"
#include "MutableArraySequence.h"
#include "SharedPtr.h"
#include <stdexcept>

template <typename Weight, typename TIdentifier>
class GraphPath {
public:
    using VertexPtr = IVertex<TIdentifier>*;
    using EdgePtr = IEdge<Weight, TIdentifier>*;
    using VertexSequence = MutableArraySequence<VertexPtr>;
    using SharedVertexSequence = SharedPtr<VertexSequence>;

private:
    SharedVertexSequence vertices_;

public:
    GraphPath() : vertices_(MakeShared<VertexSequence>()) {}
    GraphPath(const VertexSequence& vertices) : vertices_(MakeShared<VertexSequence>(vertices)) {}
    GraphPath(const SharedVertexSequence& vertices) : vertices_(vertices) {}
    ~GraphPath() = default;

    VertexPtr getStartVertex() const {
        if (vertices_->getLength() > 0) {
            return vertices_->get(0);
        }
        return nullptr; // Path is empty
    }

    VertexPtr getEndVertex() const {
        if (vertices_->getLength() > 0) {
            return vertices_->get(vertices_->getLength() - 1);
        }
        return nullptr; // Path is empty
    }

    bool containsVertex(const VertexPtr vertex) const {
        for (size_t i = 0; i < vertices_->getLength(); ++i) {
            if (vertices_->get(i)->getId() == vertex->getId()) {
                return true;
            }
        }
        return false;
    }

    bool containsEdge(const IGraph<Weight, TIdentifier>* graph, const EdgePtr edge) const {
        for (size_t i = 0; i < vertices_->getLength() - 1; ++i) {
            IVertex<TIdentifier>* fromVertex = vertices_->get(i);
            IVertex<TIdentifier>* toVertex = vertices_->get(i + 1);
            //Проверяем, что текущее ребро, это переход от fromVertex к toVertex
            if(edge->getFrom()->getId() == fromVertex->getId() && edge->getTo()->getId() == toVertex->getId())
            {
                return true;
            }
        }
        return false;
    }

    bool canConcatenate(const GraphPath<Weight, TIdentifier>& otherPath) const {
        if (getEndVertex() == nullptr || otherPath.getStartVertex() == nullptr) {
            return false; // Cannot concatenate if either path is empty
        }
        return getEndVertex()->getId() == otherPath.getStartVertex()->getId();
    }

    GraphPath<Weight, TIdentifier> concatenate(const GraphPath<Weight, TIdentifier>& otherPath) const {
        if (!canConcatenate(otherPath)) {
            throw std::invalid_argument("Paths cannot be concatenated: end vertex of the first path is not the start vertex of the second path.");
        }

        auto newVertices = MakeShared<VertexSequence>();
        // Copy vertices from the first path
        for (size_t i = 0; i < vertices_->getLength(); ++i) {
            newVertices->append(vertices_->get(i));
        }
        // Append vertices from the second path, starting from the second vertex (to avoid duplication of the connecting vertex)
        for (size_t i = 1; i < otherPath.vertices_->getLength(); ++i) {
            newVertices->append(otherPath.vertices_->get(i));
        }

        return GraphPath<Weight, TIdentifier>(newVertices);
    }

    size_t getLength() const {
        return vertices_->getLength();
    }

    VertexSequence getVertices() const {
        return *vertices_;
    }
};

#endif // GRAPHPATH_H