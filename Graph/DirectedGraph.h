#ifndef DIRECTEDGRAPH_H
#define DIRECTEDGRAPH_H

#include "IGraph.h"
#include "HashTableDictionary.h"
#include "MutableArraySequence.h"
#include "Vertex.h"
#include "Edge.h"

template <typename Weight>
class DirectedGraph : public IGraph<Weight> {
private:
    using VertexMap = HashTableDictionary<size_t, IVertex*>;
    using AdjacencyList = HashTableDictionary<size_t, HashTableDictionary<size_t, Edge<Weight>*>>;

    AdjacencyList adjList_;
    VertexMap vertexMap_;

    void fillOrder(IVertex* vertex, HashTableDictionary<size_t, bool>& visited,
                   MutableArraySequence<IVertex*>& stack) const;

    DirectedGraph<Weight> getTransposedGraph() const;

public:
    ~DirectedGraph() override;

    void dfs(IVertex* vertex, HashTableDictionary<size_t, bool>& visited,
             MutableArraySequence<IVertex*>& component) const;

    void addVertex(IVertex* vertex) override;

    void removeVertex(IVertex* vertex) override;

    void addEdge(IVertex* fromVertex, IVertex* toVertex, Weight weight) override;

    void removeEdge(IVertex* from, IVertex* to) override;

    MutableArraySequence<IVertex*> getVertices() const override;

    MutableArraySequence<IEdge<Weight>*> getEdges(IVertex* vertex) const override;

    IVertex* getVertexById(size_t vertexId) const override;

    bool hasVertex(IVertex* vertex) const override;

    bool hasEdge(IVertex* fromVertex, IVertex* toVertex) const override;
};

// Реализация DirectedGraph

template <typename Weight>
void DirectedGraph<Weight>::fillOrder(IVertex* vertex, HashTableDictionary<size_t, bool>& visited,
                                 MutableArraySequence<IVertex*>& stack) const {
    visited.add(vertex->getId(), true);
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex* neighbor = edges.get(i)->getTo();
        if (!visited.get(neighbor->getId())) {
            fillOrder(neighbor, visited, stack);
        }
    }
    stack.append(vertex);
}

template <typename Weight>
DirectedGraph<Weight> DirectedGraph<Weight>::getTransposedGraph() const {
    DirectedGraph<Weight> transposedGraph;
    auto vertices = getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex* vertex = vertices.get(i);
        auto edges = getEdges(vertex);
        for (size_t j = 0; j < edges.getLength(); ++j) {
            transposedGraph.addEdge(edges.get(j)->getTo(), vertex, edges.get(j)->getWeight());
        }
    }
    return transposedGraph;
}

template <typename Weight>
DirectedGraph<Weight>::~DirectedGraph() {
    auto vertices = DirectedGraph<Weight>::getVertices();
    MutableArraySequence<IEdge<Weight>*> edgesToDelete;
    MutableArraySequence<IVertex*> verticesToDelete;

    for (size_t i = 0; i < vertices.getLength(); ++i) {
        auto edges = DirectedGraph<Weight>::getEdges(vertices.get(i));
        for (size_t j = 0; j < edges.getLength(); ++j) {
            edgesToDelete.append(edges.get(j));
        }
        verticesToDelete.append(vertices.get(i));
    }

    for (size_t i = 0; i < edgesToDelete.getLength(); ++i) {
         edgesToDelete.removeAt(i);
    }

    for (size_t i = 0; i < verticesToDelete.getLength(); ++i) {
        verticesToDelete.removeAt(i);
    }
}

template <typename Weight>
void DirectedGraph<Weight>::dfs(IVertex* vertex, HashTableDictionary<size_t, bool>& visited,
                           MutableArraySequence<IVertex*>& component) const {
    visited.add(vertex->getId(), true);
    component.append(vertex);
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex* neighbor = edges.get(i)->getTo();
        if (!visited.get(neighbor->getId())) {
            dfs(neighbor, visited, component);
        }
    }
}

template <typename Weight>
void DirectedGraph<Weight>::addVertex(IVertex* vertex) {
    if (!vertexMap_.containsKey(vertex->getId())) {
        vertexMap_.add(vertex->getId(), vertex);
        adjList_.add(vertex->getId(), HashTableDictionary<size_t, Edge<Weight>*>());
    }
}

template <typename Weight>
void DirectedGraph<Weight>::removeVertex(IVertex* vertex) {
    if (!vertexMap_.containsKey(vertex->getId())) {
        return; // Вершины нет в графе
    }

    // Удаляем все ребра, связанные с вершиной
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        delete edges.get(i);
    }

    adjList_.remove(vertex->getId());
    vertexMap_.remove(vertex->getId());
}

template <typename Weight>
void DirectedGraph<Weight>::addEdge(IVertex* fromVertex, IVertex* toVertex, Weight weight) {
    if (!vertexMap_.containsKey(fromVertex->getId())) {
        addVertex(fromVertex);
    }
    if (!vertexMap_.containsKey(toVertex->getId())) {
        addVertex(toVertex);
    }
    if (adjList_.containsKey(fromVertex->getId())) {
        auto fromEdges = adjList_.get(fromVertex->getId());
    } else {
    }

    adjList_.get(fromVertex->getId()).add(toVertex->getId(), new Edge<Weight>(fromVertex, toVertex, weight));
}

template <typename Weight>
void DirectedGraph<Weight>::removeEdge(IVertex* fromVertex, IVertex* toVertex) {
    if (!hasEdge(fromVertex, toVertex)) {
        return; // Ребра нет в графе
    }
    Edge<Weight>* edgeToRemove = nullptr;
    auto fromVertexEdges = adjList_.get(fromVertex->getId());
    auto edgeItem = fromVertexEdges.getAllItems();
    for(size_t i = 0; i < edgeItem->getLength(); ++i){
        if(edgeItem->get(i).first == toVertex->getId()){
            edgeToRemove = edgeItem->get(i).second;
        }
    }
    fromVertexEdges.remove(toVertex->getId());

    if (edgeToRemove != nullptr) {
        delete edgeToRemove;
    }
}

template <typename Weight>
MutableArraySequence<IVertex*> DirectedGraph<Weight>::getVertices() const {
    auto items = vertexMap_.getAllItems();
    MutableArraySequence<IVertex*> result;
    for (size_t i = 0; i < items->getLength(); ++i) {
        result.append(items->get(i).second);
    }
    return result;
}

template <typename Weight>
MutableArraySequence<IEdge<Weight>*> DirectedGraph<Weight>::getEdges(IVertex* vertex) const {
    if (!adjList_.containsKey(vertex->getId())) {
        throw std::out_of_range("Vertex not found");
    }
    auto edges = adjList_.get(vertex->getId()).getAllItems();
    auto result = new MutableArraySequence<IEdge<Weight>*>();
    for (size_t i = 0; i < edges->getLength(); ++i) {
        result->append(edges->get(i).second);
    }
    return *result;
}

template <typename Weight>
IVertex* DirectedGraph<Weight>::getVertexById(size_t vertexId) const {
    if (vertexMap_.containsKey(vertexId)) {
        return vertexMap_.get(vertexId);
    }
    return nullptr;
}

template <typename Weight>
bool DirectedGraph<Weight>::hasVertex(IVertex* vertex) const {
    return vertexMap_.containsKey(vertex->getId());
}

template <typename Weight>
bool DirectedGraph<Weight>::hasEdge(IVertex* fromVertex, IVertex* toVertex) const {
    if (!adjList_.containsKey(fromVertex->getId())) {
        return false;
    }
    return adjList_.get(fromVertex->getId()).containsKey(toVertex->getId());
}

#endif // DIRECTEDGRAPH_H