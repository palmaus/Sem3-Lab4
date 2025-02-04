#ifndef UNDIRECTEDGRAPH_H
#define UNDIRECTEDGRAPH_H

#include "IGraph.h"
#include "HashTableDictionary.h"
#include "MutableArraySequence.h"
#include "Vertex.h"
#include "Edge.h"

template <typename Weight>
class UndirectedGraph : public IGraph<Weight> {
private:
    using VertexMap = HashTableDictionary<size_t, IVertex*>;
    using AdjacencyList = HashTableDictionary<size_t, HashTableDictionary<size_t, Edge<Weight>*>>;

    AdjacencyList adjList_;
    VertexMap vertexMap_;

    void dfs(IVertex* vertex, HashTableDictionary<size_t, bool>& visited,
             MutableArraySequence<IVertex*>& component) const;

public:
    ~UndirectedGraph() override;

    void addVertex(IVertex* vertex) override;

    void addEdge(IVertex* fromVertex, IVertex* toVertex, Weight weight) override;

    void removeVertex(IVertex* vertex) override;

    void removeEdge(IVertex* from, IVertex* to) override;

    MutableArraySequence<IVertex*> getVertices() const override;

    MutableArraySequence<IEdge<Weight>*> getEdges(IVertex* vertex) const override;

    IVertex* getVertexById(size_t vertexId) const override;

    bool hasVertex(IVertex* vertex) const override;

    bool hasEdge(IVertex* fromVertex, IVertex* toVertex) const override;

    MutableArraySequence<MutableArraySequence<IVertex*>> findConnectedComponents() const;
};

// Реализация UndirectedGraph

template <typename Weight>
void UndirectedGraph<Weight>::dfs(IVertex* vertex, HashTableDictionary<size_t, bool>& visited,
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
UndirectedGraph<Weight>::~UndirectedGraph() {
    auto vertices = getVertices();
    MutableArraySequence<IEdge<Weight>*> edgesToDelete;
    MutableArraySequence<IVertex*> verticesToDelete;

    for (size_t i = 0; i < vertices.getLength(); ++i) {
        auto edges = getEdges(vertices.get(i));
        for (size_t j = 0; j < edges.getLength(); ++j) {
            edgesToDelete.append(edges.get(j));
        }
        verticesToDelete.append(vertices.get(i));
    }

    for (size_t i = 0; i < edgesToDelete.getLength(); ++i) {
        delete edgesToDelete.get(i);
    }

    for (size_t i = 0; i < verticesToDelete.getLength(); ++i) {
        delete verticesToDelete.get(i);
    }
}

template <typename Weight>
void UndirectedGraph<Weight>::addVertex(IVertex* vertex) {
    if (!vertexMap_.containsKey(vertex->getId())) {
        vertexMap_.add(vertex->getId(), vertex);
        adjList_.add(vertex->getId(), HashTableDictionary<size_t, Edge<Weight>*>());
    }
}

template <typename Weight>
void UndirectedGraph<Weight>::removeVertex(IVertex* vertex) {
    if (!vertexMap_.containsKey(vertex->getId())) {
        return;
    }

    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        auto edge = edges.get(i);

        if (edge->getFrom()->getId() == vertex->getId()) {
            adjList_.get(edge->getTo()->getId()).remove(vertex->getId());
        } else {
            adjList_.get(edge->getFrom()->getId()).remove(vertex->getId());
        }
    }
    for (size_t i = 0; i < edges.getLength(); ++i) {
        delete edges.get(i);
    }
    adjList_.remove(vertex->getId());
    vertexMap_.remove(vertex->getId());
}

template <typename Weight>
void UndirectedGraph<Weight>::removeEdge(IVertex* fromVertex, IVertex* toVertex) {
    if (!hasEdge(fromVertex, toVertex)) {
        return; // Ребра нет в графе
    }

    // Удаляем ребро из adjList_
    Edge<Weight>* edgeToRemove = nullptr;
    auto fromVertexEdges = adjList_.get(fromVertex->getId());
    auto edgeItem = fromVertexEdges.getAllItems();
    for(size_t i = 0; i < edgeItem->getLength(); ++i){
        if(edgeItem->get(i).first == toVertex->getId()){
            edgeToRemove = edgeItem->get(i).second;
        }
    }
    fromVertexEdges.remove(toVertex->getId());

    // Удаляем ребро из списка смежности второй вершины (так как граф неориентированный)
    Edge<Weight>* edgeToRemove2 = nullptr;
    auto toVertexEdges = adjList_.get(toVertex->getId());
    auto edgeItem2 = toVertexEdges.getAllItems();
    for(size_t i = 0; i < edgeItem2->getLength(); ++i){
        if(edgeItem2->get(i).first == fromVertex->getId()){
            edgeToRemove2 = edgeItem2->get(i).second;
        }
    }
    toVertexEdges.remove(fromVertex->getId());

    // Теперь безопасно удаляем объект Edge
    if (edgeToRemove != nullptr) {
        delete edgeToRemove;
    }
}

template <typename Weight>
void UndirectedGraph<Weight>::addEdge(IVertex* fromVertex, IVertex* toVertex, Weight weight) {
    if (!vertexMap_.containsKey(fromVertex->getId())) {
        addVertex(fromVertex);
    }
    if (!vertexMap_.containsKey(toVertex->getId())) {
        addVertex(toVertex);
    }
    adjList_.get(fromVertex->getId()).add(toVertex->getId(), new Edge<Weight>(fromVertex, toVertex, weight));
    adjList_.get(toVertex->getId()).add(fromVertex->getId(), new Edge<Weight>(toVertex, fromVertex, weight)); // Undirected edge
}

template <typename Weight>
MutableArraySequence<IVertex*> UndirectedGraph<Weight>::getVertices() const {
    auto vertices = vertexMap_.getAllItems();
    auto result = new MutableArraySequence<IVertex*>();
    for (size_t i = 0; i < vertices->getLength(); ++i) {
        result->append(vertices->get(i).second);
    }
    return *result;
}

template <typename Weight>
MutableArraySequence<IEdge<Weight>*> UndirectedGraph<Weight>::getEdges(IVertex* vertex) const {
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
IVertex* UndirectedGraph<Weight>::getVertexById(size_t vertexId) const {
    if (vertexMap_.containsKey(vertexId)) {
        return vertexMap_.get(vertexId);
    }
    return nullptr;
}

template <typename Weight>
bool UndirectedGraph<Weight>::hasVertex(IVertex* vertex) const {
    return vertexMap_.containsKey(vertex->getId());
}

template <typename Weight>
bool UndirectedGraph<Weight>::hasEdge(IVertex* fromVertex, IVertex* toVertex) const {
    if (!adjList_.containsKey(fromVertex->getId())) {
        return false;
    }
    return adjList_.get(fromVertex->getId()).containsKey(toVertex->getId());
}

template <typename Weight>
MutableArraySequence<MutableArraySequence<IVertex*>>
UndirectedGraph<Weight>::findConnectedComponents() const {
    auto vertices = getVertices();
    HashTableDictionary<size_t, bool> visited;
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        visited.add(vertices.get(i)->getId(), false);
    }

    auto components = new MutableArraySequence<MutableArraySequence<IVertex*>>();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex* vertex = vertices.get(i);
        if (!visited.get(vertex->getId())) {
            auto component = new MutableArraySequence<IVertex*>();
            dfs(vertex, visited, component);
            components->append(*component);
        }
    }

    return *components;
}

#endif // UNDIRECTEDGRAPH_H