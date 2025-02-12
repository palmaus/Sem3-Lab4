#ifndef DIRECTEDGRAPH_H
#define DIRECTEDGRAPH_H

#include "IGraph.h"
#include "iostream"
#include "HashTableDictionary.h"
#include "MutableArraySequence.h"
#include "Vertex.h"
#include "Edge.h"

template <typename TWeight, typename TIdentifier>
class DirectedGraph : public IGraph<TWeight, TIdentifier> {
private:
    using VertexMap = HashTableDictionary<TIdentifier, IVertex<TIdentifier>*>;
    using AdjacencyList = HashTableDictionary<TIdentifier, HashTableDictionary<TIdentifier, Edge<TWeight, TIdentifier>*>>;

    AdjacencyList adjList_;
    VertexMap vertexMap_;

    void fillOrder(IVertex<TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                   MutableArraySequence<IVertex<TIdentifier>*>& stack) const;

    DirectedGraph<TWeight, TIdentifier> getTransposedGraph() const;

public:
    ~DirectedGraph() override;

    void dfs(IVertex<TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
             MutableArraySequence<IVertex<TIdentifier>*>& component) const;

    void addVertex(IVertex<TIdentifier>* vertex) override;

    void removeVertex(IVertex<TIdentifier>* vertex) override;

    void addEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex, TWeight weight) override;

    void removeEdge(IVertex<TIdentifier>* from, IVertex<TIdentifier>* to) override;

    MutableArraySequence<IVertex<TIdentifier>*> getVertices() const override;

    MutableArraySequence<IEdge<TWeight, TIdentifier>*> getEdges(IVertex<TIdentifier>* vertex) const override;

    IVertex<TIdentifier>* getVertexById(TIdentifier vertexId) const override;

    bool hasVertex(IVertex<TIdentifier>* vertex) const override;

    bool hasEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) const override;
};

// Реализация DirectedGraph

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::fillOrder(IVertex<TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                                 MutableArraySequence<IVertex<TIdentifier>*>& stack) const {
    if (!vertex) return; // Добавили проверку

    visited.add(vertex->getId(), true);
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TIdentifier>* neighbor = edges.get(i)->getTo();
        if (!visited.get(neighbor->getId())) {
            fillOrder(neighbor, visited, stack);
        }
    }
    stack.append(vertex);
}

template <typename TWeight, typename TIdentifier>
DirectedGraph<TWeight, TIdentifier> DirectedGraph<TWeight, TIdentifier>::getTransposedGraph() const {
    DirectedGraph<TWeight, TIdentifier> transposedGraph;
    auto vertices = getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex<TIdentifier>* vertex = vertices.get(i);
        auto edges = getEdges(vertex);
        for (size_t j = 0; j < edges.getLength(); ++j) {
            transposedGraph.addEdge(edges.get(j)->getTo(), vertex, edges.get(j)->getWeight());
        }
    }
    return transposedGraph;
}

template <typename TWeight, typename TIdentifier>
DirectedGraph<TWeight, TIdentifier>::~DirectedGraph() {
   auto vertices = getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        auto edges = getEdges(vertices.get(i));
        for (size_t j = 0; j < edges.getLength(); ++j) {
            delete edges.get(j); // Удаляем ребра
        }
    }

    for (size_t i = 0; i < vertices.getLength(); ++i) {
        delete vertices.get(i); // Удаляем вершины!
    }
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::dfs(IVertex<TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                           MutableArraySequence<IVertex<TIdentifier>*>& component) const {
     if (!vertex) return; // Добавили проверку

    visited.add(vertex->getId(), true);
    component.append(vertex);
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TIdentifier>* neighbor = edges.get(i)->getTo();
        if (!visited.get(neighbor->getId())) {
            dfs(neighbor, visited, component);
        }
    }
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::addVertex(IVertex<TIdentifier>* vertex) {
    if (!vertex) return; // Добавили проверку

    if (!vertexMap_.containsKey(vertex->getId())) {
        vertexMap_.add(vertex->getId(), vertex);
        adjList_.add(vertex->getId(), HashTableDictionary<TIdentifier, Edge<TWeight, TIdentifier>*>());
    }
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::removeVertex(IVertex<TIdentifier>* vertex) {
	if (!vertex) return;
    if (!vertexMap_.containsKey(vertex->getId())) {
        return; // Вершины нет в графе
    }

    // Удаляем все ребра, связанные с вершиной
    auto edges = getEdges(vertex);

    // Сначала удаляем ребра из списков смежности других вершин
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TIdentifier>* other = (edges.get(i)->getFrom()->getId() == vertex->getId()) ?
            edges.get(i)->getTo() : edges.get(i)->getFrom();

        if (adjList_.containsKey(other->getId())) { // Проверка нужна, т.к. other может быть удален в цикле
            adjList_.get(other->getId()).remove(vertex->getId());
        }
    }
     // Теперь безопасно удаляем сами ребра,
     for (size_t i = 0; i < edges.getLength(); ++i)
     {
         delete edges.get(i);
     }

    // Удаляем вершину из списков
    adjList_.remove(vertex->getId());
    vertexMap_.remove(vertex->getId());
     // delete vertex;  //  Удалять vertex должен тот, кто его создал (в данном случае - GUI)
}


template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::addEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex, TWeight weight) {
    if (!fromVertex || !toVertex) return;  // Добавили проверку

    if (!vertexMap_.containsKey(fromVertex->getId())) {
        addVertex(fromVertex);
    }
    if (!vertexMap_.containsKey(toVertex->getId())) {
        addVertex(toVertex);
    }
    // Проверяем, нет ли уже такого ребра.  Если есть - ничего не делаем.
    if (!hasEdge(fromVertex, toVertex)) {
        adjList_.get(fromVertex->getId()).add(toVertex->getId(), new Edge<TWeight, TIdentifier>(fromVertex, toVertex, weight));
    }
}


template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::removeEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) {
    if (!fromVertex || !toVertex) return;   // Добавили проверку
    if (!hasEdge(fromVertex, toVertex)) {
        return; // Ребра нет в графе
    }

    // Более эффективный способ удаления ребра:
     auto& fromVertexEdges = adjList_.get(fromVertex->getId()); // Получаем ссылку на словарь!
     Edge<TWeight, TIdentifier>* edgeToRemove = fromVertexEdges.get(toVertex->getId()); // Получаем указатель на ребро
      fromVertexEdges.remove(toVertex->getId());

    if (edgeToRemove != nullptr) { // Ребро могло быть уже удалено в removeVertex
        delete edgeToRemove;
    }
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IVertex<TIdentifier>*> DirectedGraph<TWeight, TIdentifier>::getVertices() const {
    auto items = vertexMap_.getAllItems();
    MutableArraySequence<IVertex<TIdentifier>*> result;
    for (size_t i = 0; i < items->getLength(); ++i) {
        result.append(items->get(i).second);
    }
    return result;
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IEdge<TWeight, TIdentifier>*> DirectedGraph<TWeight, TIdentifier>::getEdges(IVertex<TIdentifier>* vertex) const {
    if (!vertex) throw std::invalid_argument("Nullptr vertex");// Добавили проверку
    if (!adjList_.containsKey(vertex->getId())) {
        return  MutableArraySequence<IEdge<TWeight, TIdentifier>*>(); // Возвращаем пустой список
    }
    auto edges = adjList_.get(vertex->getId()).getAllItems();
    auto result = new MutableArraySequence<IEdge<TWeight, TIdentifier>*>();
    for (size_t i = 0; i < edges->getLength(); ++i) {
        result->append(edges->get(i).second);
    }
    return *result;
}

template <typename TWeight, typename TIdentifier>
IVertex<TIdentifier>* DirectedGraph<TWeight, TIdentifier>::getVertexById(TIdentifier vertexId) const {
    if (vertexMap_.containsKey(vertexId)) {
        return vertexMap_.get(vertexId);
    }
    return nullptr;
}

template <typename TWeight, typename TIdentifier>
bool DirectedGraph<TWeight, TIdentifier>::hasVertex(IVertex<TIdentifier>* vertex) const {
     if (!vertex) return false;
    return vertexMap_.containsKey(vertex->getId());
}

template <typename TWeight, typename TIdentifier>
bool DirectedGraph<TWeight, TIdentifier>::hasEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) const {
    if (!fromVertex || !toVertex) return false;
    if (!adjList_.containsKey(fromVertex->getId())) {
        return false;
    }
    return adjList_.get(fromVertex->getId()).containsKey(toVertex->getId());
}

#endif // DIRECTEDGRAPH_H