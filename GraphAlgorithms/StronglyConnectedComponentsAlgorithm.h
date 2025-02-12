#ifndef STRONGLYCONNECTEDCOMPONENTSALGORITHM_H
#define STRONGLYCONNECTEDCOMPONENTSALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include "SharedPtr.h"
#include "DirectedGraph.h"

template <typename TWeight, typename TIdentifier>
class StronglyConnectedComponentsAlgorithm : public IAlgorithm<TWeight, MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>, TIdentifier> {
private:
    void dfs(IVertex<TIdentifier>* vertex, const IGraph<TWeight, TIdentifier>* graph, HashTableDictionary<TIdentifier, bool>& visited, MutableArraySequence<IVertex<TIdentifier>*>& component) const {
        if (!vertex) return;
        visited.add(vertex->getId(), true);

        Vertex<TIdentifier>* copiedVertex = new Vertex<TIdentifier>(*dynamic_cast<Vertex<TIdentifier>*>(vertex));
        component.append(copiedVertex);

        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TIdentifier>* neighbor = edges.get(i)->getTo();
            if (neighbor && !visited.get(neighbor->getId())) {
                dfs(neighbor, graph, visited, component);
            }
        }
    }

    void fillOrder(IVertex<TIdentifier>* vertex, const IGraph<TWeight, TIdentifier>* graph, HashTableDictionary<TIdentifier, bool>& visited, MutableArraySequence<IVertex<TIdentifier>*>& stack) const {
        if(!vertex) return;
        visited.add(vertex->getId(), true);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TIdentifier>* neighbor = edges.get(i)->getTo();
            if (neighbor && !visited.get(neighbor->getId())) {
                fillOrder(neighbor, graph, visited, stack);
            }
        }
        stack.append(vertex);
    }


    IGraph<TWeight, TIdentifier>* getTransposedGraph(const IGraph<TWeight, TIdentifier>* graph) const {
        const auto* directedGraph = dynamic_cast<const DirectedGraph<TWeight, TIdentifier>*>(graph);
        if (!directedGraph) {
            throw std::runtime_error("Cannot transpose non-directed graph");
        }
        auto* transposedGraph = new DirectedGraph<TWeight, TIdentifier>();
        auto vertices = graph->getVertices();

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex<TIdentifier>* originalVertex = vertices.get(i);
            IVertex<TIdentifier>* newVertex = new Vertex<TIdentifier>(*dynamic_cast<Vertex<TIdentifier>*>(originalVertex));
            transposedGraph->addVertex(newVertex);
        }

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex<TIdentifier>* originalVertex = vertices.get(i);
            auto edges = graph->getEdges(originalVertex);

            IVertex<TIdentifier>* newFrom = transposedGraph->getVertexById(originalVertex->getId());

            for (size_t j = 0; j < edges.getLength(); ++j) {
                IVertex<TIdentifier>* newTo = transposedGraph->getVertexById(edges.get(j)->getTo()->getId());
                transposedGraph->addEdge(newTo, newFrom, edges.get(j)->getWeight());
            }
        }
        return transposedGraph;
    }

public:
    ~StronglyConnectedComponentsAlgorithm() override = default;

    SharedPtr<MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>> execute(
        const IGraph<TWeight, TIdentifier>* graph,
        IVertex<TIdentifier>* startVertex = nullptr,
        IVertex<TIdentifier>* endVertex = nullptr
    ) const override {
        const auto* directedGraph = dynamic_cast<const DirectedGraph<TWeight, TIdentifier>*>(graph);
        if (!directedGraph) {
            throw std::runtime_error("Strongly connected components algorithm can be applied to directed graphs only");
        }


        auto vertices = graph->getVertices();
        HashTableDictionary<TIdentifier, bool> visited;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }

        auto stack = MakeShared<MutableArraySequence<IVertex<TIdentifier>*>>();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            if (!visited.get(vertices.get(i)->getId())) {
                fillOrder(vertices.get(i), graph, visited, *stack);
            }
        }

        IGraph<TWeight, TIdentifier>* transposedGraph = getTransposedGraph(graph);
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }
        auto components = MakeShared<MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>>();

        for (int i = stack->getLength() - 1; i >= 0; --i) {
            IVertex<TIdentifier>* vertex = stack->get(i);
            if (!visited.get(vertex->getId())) {
                auto component = MakeShared<MutableArraySequence<IVertex<TIdentifier>*>>();
                dfs(vertex, transposedGraph, visited, *component);
                components->append(*component);
            }
        }
        delete transposedGraph;
        return components;
    }
};

#endif // STRONGLYCONNECTEDCOMPONENTSALGORITHM_H