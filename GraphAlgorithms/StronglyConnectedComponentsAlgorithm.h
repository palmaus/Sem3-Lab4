#ifndef STRONGLYCONNECTEDCOMPONENTSALGORITHM_H
#define STRONGLYCONNECTEDCOMPONENTSALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include "SharedPtr.h"
#include "DirectedGraph.h"

template <typename Weight>
class StronglyConnectedComponentsAlgorithm : public IAlgorithm<Weight, MutableArraySequence<MutableArraySequence<IVertex*>>> {
private:
    void dfs(IVertex* vertex, const IGraph<Weight>* graph, HashTableDictionary<size_t, bool>& visited, MutableArraySequence<IVertex*>& component) const {
        visited.add(vertex->getId(), true);
        component.append(vertex);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex* neighbor = edges.get(i)->getTo();
            if (!visited.get(neighbor->getId())) {
                dfs(neighbor, graph, visited, component);
            }
        }
    }

    void fillOrder(IVertex* vertex, const IGraph<Weight>* graph, HashTableDictionary<size_t, bool>& visited, MutableArraySequence<IVertex*>& stack) const {
        visited.add(vertex->getId(), true);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex* neighbor = edges.get(i)->getTo();
            if (!visited.get(neighbor->getId())) {
                fillOrder(neighbor, graph, visited, stack);
            }
        }
        stack.append(vertex);
    }


    IGraph<Weight>* getTransposedGraph(const IGraph<Weight>* graph) const {
        // Проверка, является ли граф DirectedGraph (должен быть более надежный способ)
        const DirectedGraph<Weight>* directedGraph = dynamic_cast<const DirectedGraph<Weight>*>(graph);
        if (!directedGraph) {
            throw std::runtime_error("Cannot transpose non-directed graph");
        }
        DirectedGraph<Weight>* transposedGraph = new DirectedGraph<Weight>();
        auto vertices = graph->getVertices();

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex* vertex = vertices.get(i);
            transposedGraph->addVertex(vertex);
        }
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex* vertex = vertices.get(i);
            auto edges = graph->getEdges(vertex);
            for (size_t j = 0; j < edges.getLength(); ++j) {
                transposedGraph->addEdge(edges.get(j)->getTo(), vertex, edges.get(j)->getWeight());
            }
        }
        return transposedGraph;
    }

public:
    ~StronglyConnectedComponentsAlgorithm() override = default;

    SharedPtr<MutableArraySequence<MutableArraySequence<IVertex*>>> execute(
        const IGraph<Weight>* graph,
        IVertex* startVertex = nullptr,
        IVertex* endVertex = nullptr
    ) const override {
        const auto* directedGraph = dynamic_cast<const DirectedGraph<Weight>*>(graph);
        if (!directedGraph) {
            throw std::runtime_error("Strongly connected components algorithm can be applied to directed graphs only");
        }


        auto vertices = graph->getVertices();
        HashTableDictionary<size_t, bool> visited;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }

        auto stack = MakeShared<MutableArraySequence<IVertex*>>();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            if (!visited.get(vertices.get(i)->getId())) {
                fillOrder(vertices.get(i), graph, visited, *stack);
            }
        }

        IGraph<Weight>* transposedGraph = getTransposedGraph(graph);
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }
        auto components = MakeShared<MutableArraySequence<MutableArraySequence<IVertex*>>>();
        for (int i = stack->getLength() - 1; i >= 0; --i) {
            IVertex* vertex = stack->get(i);
            if (!visited.get(vertex->getId())) {
                auto component = MakeShared<MutableArraySequence<IVertex*>>();
                dfs(vertex, transposedGraph, visited, *component);
                components->append(*component);
            }
        }
        delete transposedGraph; // Удаляем транспонированный граф
        return components;
    }
};

#endif // STRONGLYCONNECTEDCOMPONENTSALGORITHM_H