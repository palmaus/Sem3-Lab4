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
    using VertexMap = HashTableDictionary<TIdentifier, IVertex<TWeight, TIdentifier>*>;
    VertexMap vertexMap_;

    void dfs(IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
             MutableArraySequence<IVertex<TWeight, TIdentifier>*>& component) const;

public:
    ~UndirectedGraph() override;
    void addVertex(IVertex<TWeight, TIdentifier>* vertex) override;
    void addEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex, TWeight weight) override;
    void removeVertex(IVertex<TWeight, TIdentifier>* vertex) override;
    void removeEdge(IVertex<TWeight, TIdentifier>* from, IVertex<TWeight, TIdentifier>* to) override;
    MutableArraySequence<IVertex<TWeight, TIdentifier>*> getVertices() const override;
    MutableArraySequence<IEdge<TWeight, TIdentifier>*> getEdges(IVertex<TWeight, TIdentifier>* vertex) const override;
    IVertex<TWeight, TIdentifier>* getVertexById(TIdentifier vertexId) const override;
    bool hasVertex(IVertex<TWeight, TIdentifier>* vertex) const override;
    bool hasEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) const override;
    MutableArraySequence<MutableArraySequence<IVertex<TWeight, TIdentifier>*>> findConnectedComponents() const;
};

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::dfs(IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                             MutableArraySequence<IVertex<TWeight, TIdentifier>*>& component) const {
    if (!vertex) return;
    visited.add(vertex->getId(), true);
    component.append(vertex);
    auto edges = getEdges(vertex);
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TWeight, TIdentifier>* neighbor = nullptr;
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

        delete vertices.get(i); // Удаляем вершины!
    }
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::addVertex(IVertex<TWeight, TIdentifier>* vertex) {
    if (!vertex) return;
    if (!vertexMap_.containsKey(vertex->getId())) {
        vertexMap_.add(vertex->getId(), vertex);
    }
}
template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::removeVertex(IVertex<TWeight, TIdentifier>* vertex) {
	if (!vertex || !vertexMap_.containsKey(vertex->getId())) {
        return;
    }

    // Удаляем все ребра, инцидентные вершине.
    auto edges = getEdges(vertex);
    while (edges.getLength() > 0) {
        auto edge = edges.get(0);
        IVertex<TWeight, TIdentifier>* other = (edge->getFrom() == vertex) ? edge->getTo() : edge->getFrom();
        removeEdge(vertex, other);
    }

    vertexMap_.remove(vertex->getId());
   // delete vertex;  //  Удалять должен тот, кто создал
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::removeEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) {
	if (!fromVertex || !toVertex || !hasEdge(fromVertex, toVertex)) {
        return;
    }
    auto fromVertexEdges = dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->getOutgoingEdges();

    for (size_t i = 0; i < fromVertexEdges.getLength(); i++)
    {
          auto edge = fromVertexEdges.get(i);

          // Проверяем, что ребро соединяет нужные вершины в правильном направлении
          if((edge->getFrom()->getId() == fromVertex->getId() && edge->getTo()->getId() == toVertex->getId()))
          {
                dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->removeOutgoingEdge(edge);
                dynamic_cast<Vertex<TWeight, TIdentifier>*>(toVertex)->removeIncomingEdge(edge);

                delete edge;
                return;
          }

          if((edge->getTo()->getId() == fromVertex->getId() && edge->getFrom()->getId() == toVertex->getId()))
          {
             dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->removeIncomingEdge(edge);
             dynamic_cast<Vertex<TWeight, TIdentifier>*>(toVertex)->removeOutgoingEdge(edge);
             delete edge;
             return;
          }
    }
}

template <typename TWeight, typename TIdentifier>
void UndirectedGraph<TWeight, TIdentifier>::addEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex, TWeight weight) {
   if (!fromVertex || !toVertex) return;

    if (!vertexMap_.containsKey(fromVertex->getId())) {
        addVertex(fromVertex);
    }
    if (!vertexMap_.containsKey(toVertex->getId())) {
        addVertex(toVertex);
    }

    auto edge1 = new Edge<TWeight, TIdentifier>(fromVertex, toVertex, weight);
    auto edge2 = new Edge<TWeight, TIdentifier>(toVertex, fromVertex, weight);

    dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->addOutgoingEdge(edge1);
    dynamic_cast<Vertex<TWeight, TIdentifier>*>(toVertex)->addIncomingEdge(edge1);

    dynamic_cast<Vertex<TWeight, TIdentifier>*>(toVertex)->addOutgoingEdge(edge2);
    dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->addIncomingEdge(edge2);
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IVertex<TWeight, TIdentifier>*> UndirectedGraph<TWeight, TIdentifier>::getVertices() const {
    auto items = vertexMap_.getAllItems();
    MutableArraySequence<IVertex<TWeight, TIdentifier>*> result;
    for (size_t i = 0; i < items->getLength(); ++i) {
        result.append(items->get(i).second);
    }
    return result;
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IEdge<TWeight, TIdentifier>*> UndirectedGraph<TWeight, TIdentifier>::getEdges(IVertex<TWeight, TIdentifier>* vertex) const {
     if (!vertex) throw std::invalid_argument("Nullptr vertex");
	 MutableArraySequence<IEdge<TWeight, TIdentifier>*> allEdges;
     auto outgoingEdges = vertex->getOutgoingEdges();
	 auto incomingEdges = vertex->getIncomingEdges();
     for (size_t i = 0; i < outgoingEdges.getLength(); ++i) {
          allEdges.append(outgoingEdges.get(i));
     }
     return allEdges;
}


template <typename TWeight, typename TIdentifier>
IVertex<TWeight, TIdentifier>* UndirectedGraph<TWeight, TIdentifier>::getVertexById(TIdentifier vertexId) const {
    if (vertexMap_.containsKey(vertexId)) {
        return vertexMap_.get(vertexId);
    }
    return nullptr;
}

template <typename TWeight, typename TIdentifier>
bool UndirectedGraph<TWeight, TIdentifier>::hasVertex(IVertex<TWeight, TIdentifier>* vertex) const {
    if (!vertex) return false;
    return vertexMap_.containsKey(vertex->getId());
}

template <typename TWeight, typename TIdentifier>
bool UndirectedGraph<TWeight, TIdentifier>::hasEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) const {
	if (!fromVertex || !toVertex) return false;
    auto outgoingEdges = fromVertex->getOutgoingEdges();

    for (size_t i = 0; i < outgoingEdges.getLength(); ++i) {

        if (outgoingEdges.get(i)->getTo()->getId() == toVertex->getId() )
        {
            return true; // Нашлось ребро от fromVertex к toVertex
        }
         if(outgoingEdges.get(i)->getFrom()->getId() == toVertex->getId())
         {
            return true;
         }
    }

    return false;
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>
UndirectedGraph<TWeight, TIdentifier>::findConnectedComponents() const {
    auto vertices = getVertices();
    HashTableDictionary<TIdentifier, bool> visited;
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        visited.add(vertices.get(i)->getId(), false);
    }

    auto components = new MutableArraySequence<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex<TWeight, TIdentifier>* vertex = vertices.get(i);
        if (!visited.get(vertex->getId())) {
            auto component = new MutableArraySequence<IVertex<TWeight, TIdentifier>*>();
            dfs(vertex, visited, component);
            components->append(*component);
        }
    }

    return *components;
}

#endif