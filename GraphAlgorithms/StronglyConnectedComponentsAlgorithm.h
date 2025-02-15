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
class StronglyConnectedComponentsAlgorithm : public IAlgorithm<TWeight, MutableArraySequence<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>, TIdentifier> {
private:

    // В StronglyConnectedComponentsAlgorithm.h

    // Внутри метода dfs:
    void dfs(IVertex<TWeight, TIdentifier>* vertex, const IGraph<TWeight, TIdentifier>* graph, HashTableDictionary<TIdentifier, bool>& visited, MutableArraySequence<IVertex<TWeight, TIdentifier>*>& component) const {
        if (!vertex) {
            std::cerr << "DFS: Ошибка! vertex is nullptr!" << std::endl;
            return;
        }
        visited.add(vertex->getId(), true);
        component.append(vertex);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* neighbor = edges.get(i)->getTo();
            if (!neighbor) {
                continue; // Важно: Пропустить nullptr соседа, чтобы не было краша
            }
            if (!visited.get(neighbor->getId())) {
                dfs(neighbor, graph, visited, component);
            }
        }
    }

    // Внутри метода fillOrder:
    void fillOrder(IVertex<TWeight, TIdentifier>* vertex, const IGraph<TWeight, TIdentifier>* graph, HashTableDictionary<TIdentifier, bool>& visited, MutableArraySequence<IVertex<TWeight, TIdentifier>*>& stack) const {
        if (!vertex) {
            return;
        }
        visited.add(vertex->getId(), true);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* neighbor = edges.get(i)->getTo();
            if (!neighbor) {
                continue; // Важно: Пропустить nullptr соседа, чтобы не было краша
            }
            if (!visited.get(neighbor->getId())) {
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
            IVertex<TWeight, TIdentifier>* originalVertex = vertices.get(i);
            transposedGraph->addVertex(new Vertex<TWeight, TIdentifier>(originalVertex->getId()));
        }

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* originalVertex = vertices.get(i);
            auto outgoingEdges = originalVertex->getOutgoingEdges();

            for (size_t j = 0; j < outgoingEdges.getLength(); ++j) {
                IEdge<TWeight, TIdentifier>* originalEdge = outgoingEdges.get(j);
                IVertex<TWeight, TIdentifier>* fromVertexOriginal = originalEdge->getFrom();
                IVertex<TWeight, TIdentifier>* toVertexOriginal = originalEdge->getTo();


                IVertex<TWeight, TIdentifier>* transposedFromVertex = transposedGraph->getVertexById(toVertexOriginal->getId());
                IVertex<TWeight, TIdentifier>* transposedToVertex = transposedGraph->getVertexById(fromVertexOriginal->getId());


                if (transposedFromVertex && transposedToVertex)
                {
                    transposedGraph->addEdge(transposedFromVertex, transposedToVertex, originalEdge->getWeight());
                }
                 else
                 {
                     std::cerr << "Warning: Vertex not found in transposed graph during edge addition! FromID: " << (transposedFromVertex ? transposedFromVertex->getId() : -1)
                               << ", ToID: " << (transposedToVertex ? transposedToVertex->getId() : -1) << std::endl;
                 }
            }
        }

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* v = vertices.get(i);
            auto edges = v->getOutgoingEdges();
        }

        auto transposedVertices = transposedGraph->getVertices();
        for (size_t i = 0; i < transposedVertices.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* v = transposedVertices.get(i);
            auto edges = v->getOutgoingEdges();
        }

        return transposedGraph;
    }

public:
    ~StronglyConnectedComponentsAlgorithm() override = default;

    SharedPtr<MutableArraySequence<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>> execute(
        const IGraph<TWeight, TIdentifier>* graph,
        IVertex<TWeight, TIdentifier>* startVertex = nullptr,
        IVertex<TWeight, TIdentifier>* endVertex = nullptr
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
        auto stack = MakeShared<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            if (!visited.get(vertices.get(i)->getId())) {
                fillOrder(vertices.get(i), graph, visited, *stack);
            }
        }
        IGraph<TWeight, TIdentifier>* transposedGraph = getTransposedGraph(graph);

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }
        auto components = MakeShared<MutableArraySequence<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>>();

        for (int i = stack->getLength() - 1; i >= 0; --i) {
            IVertex<TWeight, TIdentifier>* vertex = stack->get(i);

            if (!visited.get(vertex->getId())) {
                auto component = MakeShared<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>();
                dfs(transposedGraph->getVertexById(vertex->getId()), transposedGraph, visited, *component);
                components->append(*component);
            }
        }
        return components;
    }
};

#endif // STRONGLYCONNECTEDCOMPONENTSALGORITHM_H