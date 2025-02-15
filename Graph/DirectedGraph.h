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
    using VertexMap = HashTableDictionary<TIdentifier, IVertex<TWeight, TIdentifier>*>;
    VertexMap vertexMap_;

    void fillOrder(IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                   MutableArraySequence<IVertex<TWeight, TIdentifier>*>& stack) const;

    DirectedGraph<TWeight, TIdentifier> getTransposedGraph() const;

    void dfs(IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
             MutableArraySequence<IVertex<TWeight, TIdentifier>*>& component) const;

public:
    ~DirectedGraph() override;

    void addVertex(IVertex<TWeight, TIdentifier>* vertex) override;
    void removeVertex(IVertex<TWeight, TIdentifier>* vertex) override;
    void addEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex, TWeight weight) override;
    void removeEdge(IVertex<TWeight, TIdentifier>* from, IVertex<TWeight, TIdentifier>* to) override;
    MutableArraySequence<IVertex<TWeight, TIdentifier>*> getVertices() const override;
    MutableArraySequence<IEdge<TWeight, TIdentifier>*> getEdges(IVertex<TWeight, TIdentifier>* vertex) const override;
    IVertex<TWeight, TIdentifier>* getVertexById(TIdentifier vertexId) const override;
    bool hasVertex(IVertex<TWeight, TIdentifier>* vertex) const override;
    bool hasEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) const override;
};

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::fillOrder(IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                                 MutableArraySequence<IVertex<TWeight, TIdentifier>*>& stack) const {
    if (!vertex) return;

    visited.add(vertex->getId(), true);
    auto edges = getEdges(vertex); // Исп. getEdges
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TWeight, TIdentifier>* neighbor = edges.get(i)->getTo();
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
        transposedGraph.addVertex(new Vertex<TWeight, TIdentifier>(vertices.get(i)->getId())); //  Добавляем верш.
    }
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex<TWeight, TIdentifier>* vertex = vertices.get(i);
        auto edges = getEdges(vertex);
        for (size_t j = 0; j < edges.getLength(); ++j) {
            auto edge = edges.get(j);
            IVertex<TWeight, TIdentifier>* from = transposedGraph.getVertexById(edge->getTo()->getId());
            IVertex<TWeight, TIdentifier>* to = transposedGraph.getVertexById(edge->getFrom()->getId());
            transposedGraph.addEdge(from, to, edge->getWeight());
        }
    }
    return transposedGraph;
}


template <typename TWeight, typename TIdentifier>
DirectedGraph<TWeight, TIdentifier>::~DirectedGraph() {
    auto vertices = getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        delete vertices.get(i); // Удаляем вершины!
    }
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::dfs(IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited,
                           MutableArraySequence<IVertex<TWeight, TIdentifier>*>& component) const {
     if (!vertex) return;

    visited.add(vertex->getId(), true);
    component.append(vertex);
    auto edges = vertex->getOutgoingEdges(); // Исп. getOutgoingEdges
    for (size_t i = 0; i < edges.getLength(); ++i) {
        IVertex<TWeight, TIdentifier>* neighbor = edges.get(i)->getTo();
        if (!visited.get(neighbor->getId())) {
            dfs(neighbor, visited, component);
        }
    }
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::addVertex(IVertex<TWeight, TIdentifier>* vertex) {
    if (!vertex) return;

    if (!vertexMap_.containsKey(vertex->getId())) {
        vertexMap_.add(vertex->getId(), vertex);
    }
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::removeVertex(IVertex<TWeight, TIdentifier>* vertex) {
    if (!vertex || !vertexMap_.containsKey(vertex->getId())) {
        return;
    }

    // Удаляем входящие ребра
    auto incomingEdges = vertex->getIncomingEdges();
    while(incomingEdges.getLength() > 0) {
      auto edge = incomingEdges.get(0);
        removeEdge(edge->getFrom(), vertex);
    }

    // Удаляем исходящие ребра
    auto outgoingEdges = vertex->getOutgoingEdges();

    while(outgoingEdges.getLength() > 0) {
       auto edge = outgoingEdges.get(0);
        removeEdge(vertex, edge->getTo());
    }


    vertexMap_.remove(vertex->getId());
     // delete vertex;  //  Удалять должен тот, кто создал
}


template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::addEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex, TWeight weight) {
    if (!fromVertex || !toVertex) return;

    if (!vertexMap_.containsKey(fromVertex->getId())) {
        addVertex(fromVertex);
    }
    if (!vertexMap_.containsKey(toVertex->getId())) {
        addVertex(toVertex);
    }

    auto edge = new Edge<TWeight, TIdentifier>(fromVertex, toVertex, weight);

    dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->addOutgoingEdge(edge);
    dynamic_cast<Vertex<TWeight, TIdentifier>*>(toVertex)->addIncomingEdge(edge); // Добавляем ТОЛЬКО входящее!
}

template <typename TWeight, typename TIdentifier>
void DirectedGraph<TWeight, TIdentifier>::removeEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) {
 if (!fromVertex || !toVertex || !hasEdge(fromVertex, toVertex)) {
        return;
    }

    auto fromVertexOutgoingEdges = dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->getOutgoingEdges();

	 for (size_t i = 0; i < fromVertexOutgoingEdges.getLength(); i++)
	 {
		  auto edge = fromVertexOutgoingEdges.get(i);
		  if(edge->getTo() == toVertex)
		  {
            dynamic_cast<Vertex<TWeight, TIdentifier>*>(fromVertex)->removeOutgoingEdge(edge);
            dynamic_cast<Vertex<TWeight, TIdentifier>*>(toVertex)->removeIncomingEdge(edge);
				delete edge;
				return;
		  }
	 }
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IVertex<TWeight, TIdentifier>*> DirectedGraph<TWeight, TIdentifier>::getVertices() const {
    auto items = vertexMap_.getAllItems();
    MutableArraySequence<IVertex<TWeight, TIdentifier>*> result;
    for (size_t i = 0; i < items->getLength(); ++i) {
        result.append(items->get(i).second);
    }
    return result;
}

template <typename TWeight, typename TIdentifier>
MutableArraySequence<IEdge<TWeight, TIdentifier>*> DirectedGraph<TWeight, TIdentifier>::getEdges(IVertex<TWeight, TIdentifier>* vertex) const {
     if (!vertex) throw std::invalid_argument("Nullptr vertex");
     MutableArraySequence<IEdge<TWeight, TIdentifier>*> allEdges;
     auto outgoingEdges = vertex->getOutgoingEdges();
     auto incomingEdges = vertex->getIncomingEdges();
     for (size_t i = 0; i < outgoingEdges.getLength(); ++i) {
         allEdges.append(outgoingEdges.get(i));
     }
	 for (size_t i = 0; i < incomingEdges.getLength(); i++)
	 {
		  allEdges.append(incomingEdges.get(i));
	 }
     return allEdges;
}

template <typename TWeight, typename TIdentifier>
IVertex<TWeight, TIdentifier>* DirectedGraph<TWeight, TIdentifier>::getVertexById(TIdentifier vertexId) const {
    if (vertexMap_.containsKey(vertexId)) {
        return vertexMap_.get(vertexId);
    }
    return nullptr;
}

template <typename TWeight, typename TIdentifier>
bool DirectedGraph<TWeight, TIdentifier>::hasVertex(IVertex<TWeight, TIdentifier>* vertex) const {
     if (!vertex) return false;
    return vertexMap_.containsKey(vertex->getId());
}

template <typename TWeight, typename TIdentifier>
bool DirectedGraph<TWeight, TIdentifier>::hasEdge(IVertex<TWeight, TIdentifier>* fromVertex, IVertex<TWeight, TIdentifier>* toVertex) const {
    if (!fromVertex || !toVertex) return false;

    auto outgoingEdges = fromVertex->getOutgoingEdges();
    for (size_t i = 0; i < outgoingEdges.getLength(); ++i) {
        if (outgoingEdges.get(i)->getTo()->getId() == toVertex->getId()) {
            return true;
        }
    }
    return false;
}

#endif