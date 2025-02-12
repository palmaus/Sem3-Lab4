#ifndef UNDIRECTEDGRAPH_H
#define UNDIRECTEDGRAPH_H

#include "IGraph.h"
#include "HashTableDictionary.h"
#include "MutableArraySequence.h"
#include "Vertex.h"
#include "Edge.h"

template <typename TWeight, typename TIdentifier>
class UndirectedGraph : public IGraph<TWeight, TIdentifier> {
private:
    using VertexMap = HashTableDictionary<TIdentifier, IVertex<TIdentifier>*>;
    using AdjacencyList = HashTableDictionary<TIdentifier, HashTableDictionary<TIdentifier, Edge<TWeight, TIdentifier>*>>;

    AdjacencyList adjList_;
    VertexMap vertexMap_;

    void dfs(IVertex<TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
             MutableArraySequence<IVertex<TIdentifier>*>& component) const;

public:
    ~UndirectedGraph() override;

    void addVertex(IVertex<TIdentifier>* vertex) override;

    void addEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex, TWeight weight) override;

    void removeVertex(IVertex<TIdentifier>* vertex) override;

    void removeEdge(IVertex<TIdentifier>* from, IVertex<TIdentifier>* to) override;

    MutableArraySequence<IVertex<TIdentifier>*> getVertices() const override;

    MutableArraySequence<IEdge<TWeight, TIdentifier>*> getEdges(IVertex<TIdentifier>* vertex) const override;

    IVertex<TIdentifier>* getVertexById(TIdentifier vertexId) const override;

    bool hasVertex(IVertex<TIdentifier>* vertex) const override;

    bool hasEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) const override;

    MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>> findConnectedComponents() const;
};

// Реализация UndirectedGraph

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::dfs(IVertex<TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                             MutableArraySequence<IVertex<TIdentifier>*>& component) const {
     if (!vertex) return; // Добавили проверку
    visited.add(vertex->getId(), true);
    component.append(vertex);
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
		  IVertex<TIdentifier>* neighbor = nullptr;
        if(edges.get(i)->getFrom()->getId() == vertex->getId())
				neighbor = edges.get(i)->getTo();
		  else
			  neighbor = edges.get(i)->getFrom();

        if (!visited.get(neighbor->getId())) {
            dfs(neighbor, visited, component);
        }
    }
}

template <typename TWeight, typename TIdentifier>
UndirectedGraph<TWeight, TIdentifier>::~UndirectedGraph() {
    auto vertices = getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        auto edges = getEdges(vertices.get(i));
        for (size_t j = 0; j < edges.getLength(); ++j) {
            delete edges.get(j);  // Удаляем ребра
        }
    }
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        delete vertices.get(i); // Удаляем вершины!
    }
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::addVertex(IVertex<TIdentifier>* vertex) {
    if (!vertex) return;
    if (!vertexMap_.containsKey(vertex->getId())) {
        vertexMap_.add(vertex->getId(), vertex);
        adjList_.add(vertex->getId(), HashTableDictionary<TIdentifier, Edge<TWeight, TIdentifier>*>());
    }
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::removeVertex(IVertex<TIdentifier>* vertex) {
	if (!vertex) return; //Добавили проверку
    if (!vertexMap_.containsKey(vertex->getId())) {
        return;
    }

    auto edges = getEdges(vertex);

    // Сначала удаляем ребра из списков смежности других вершин
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TIdentifier>* other = (edges.get(i)->getFrom()->getId() == vertex->getId()) ?
            edges.get(i)->getTo() : edges.get(i)->getFrom();
         if (adjList_.containsKey(other->getId()))
         {
           adjList_.get(other->getId()).remove(vertex->getId()); // УДАЛЯЕМ ИЗ СПИСКОВ СОСЕДЕЙ!
         }

    }

    // Теперь безопасно удаляем сами ребра
      for (size_t i = 0; i < edges.getLength(); ++i)
     {
         delete edges.get(i);
     }

    adjList_.remove(vertex->getId());
    vertexMap_.remove(vertex->getId());
    // delete vertex;  // Удалять vertex должен тот, кто его создал!
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::removeEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) {
	if (!fromVertex || !toVertex) return;
    if (!hasEdge(fromVertex, toVertex)) {
        return; // Ребра нет в графе
    }

    // Удаляем ребро из adjList_ (в обоих направлениях!)
     auto& fromVertexEdges = adjList_.get(fromVertex->getId());
    Edge<TWeight, TIdentifier>* edgeToRemove = fromVertexEdges.get(toVertex->getId());
    fromVertexEdges.remove(toVertex->getId());

	 auto& toVertexEdges = adjList_.get(toVertex->getId());
	 toVertexEdges.remove(fromVertex->getId());

    // Теперь безопасно удаляем объект Edge
    if (edgeToRemove != nullptr) { // проверка на nullptr
        delete edgeToRemove;
    }
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::addEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex, TWeight weight) {
   if (!fromVertex || !toVertex) return;  // Добавили проверку

    if (!vertexMap_.containsKey(fromVertex->getId())) {
        addVertex(fromVertex);
    }
    if (!vertexMap_.containsKey(toVertex->getId())) {
        addVertex(toVertex);
    }
    // Добавляем ребро в обоих направлениях
    adjList_.get(fromVertex->getId()).add(toVertex->getId(), new Edge<TWeight, TIdentifier>(fromVertex, toVertex, weight));
    adjList_.get(toVertex->getId()).add(fromVertex->getId(), new Edge<TWeight, TIdentifier>(toVertex, fromVertex, weight)); // Undirected edge
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IVertex<TIdentifier>*> UndirectedGraph<TWeight, TIdentifier>::getVertices() const {
    auto items = vertexMap_.getAllItems();
    MutableArraySequence<IVertex<TIdentifier>*> result;
    for (size_t i = 0; i < items->getLength(); ++i) {
        result.append(items->get(i).second);
    }
    return result;
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IEdge<TWeight, TIdentifier>*> UndirectedGraph<TWeight, TIdentifier>::getEdges(IVertex<TIdentifier>* vertex) const {
    if (!vertex) throw std::invalid_argument("Nullptr vertex");
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
IVertex<TIdentifier>* UndirectedGraph<TWeight, TIdentifier>::getVertexById(TIdentifier vertexId) const {
    if (vertexMap_.containsKey(vertexId)) {
        return vertexMap_.get(vertexId);
    }
    return nullptr;
}

template <typename TWeight, typename TIdentifier>
bool UndirectedGraph<TWeight, TIdentifier>::hasVertex(IVertex<TIdentifier>* vertex) const {
    if (!vertex) return false;
    return vertexMap_.containsKey(vertex->getId());
}

template <typename TWeight, typename TIdentifier>
bool UndirectedGraph<TWeight, TIdentifier>::hasEdge(IVertex<TIdentifier>* fromVertex, IVertex<TIdentifier>* toVertex) const {
    if (!fromVertex || !toVertex) return false; // Добавили проверку

    if (!adjList_.containsKey(fromVertex->getId())) {
        return false;
    }
    return adjList_.get(fromVertex->getId()).containsKey(toVertex->getId());
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>
UndirectedGraph<TWeight, TIdentifier>::findConnectedComponents() const {
    auto vertices = getVertices();
    HashTableDictionary<TIdentifier, bool> visited;
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        visited.add(vertices.get(i)->getId(), false);
    }

    auto components = new MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex<TIdentifier>* vertex = vertices.get(i);
        if (!visited.get(vertex->getId())) {
            auto component = new MutableArraySequence<IVertex<TIdentifier>*>();
            dfs(vertex, visited, component);
            components->append(*component);
        }
    }

    return *components;
}

#endif // UNDIRECTEDGRAPH_H