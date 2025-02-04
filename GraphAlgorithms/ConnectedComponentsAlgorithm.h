#ifndef CONNECTEDCOMPONENTSALGORITHM_H
#define CONNECTEDCOMPONENTSALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include "SharedPtr.h"
#include <functional>

template <typename Weight>
class ConnectedComponentsAlgorithm : public IAlgorithm<Weight, MutableArraySequence<MutableArraySequence<IVertex*>>> {
private:
    void dfs(IVertex* vertex, const IGraph<Weight>* graph, HashTableDictionary<size_t, bool>& visited, MutableArraySequence<IVertex*>& component) const {
        visited.add(vertex->getId(), true);
        component.append(vertex);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex* neighbor = nullptr;
            if (edges.get(i)->getFrom()->getId() == vertex->getId()) {
                neighbor = edges.get(i)->getTo();
            } else {
                neighbor = edges.get(i)->getFrom();
            }
            if (!visited.get(neighbor->getId())) {
                dfs(neighbor, graph, visited, component);
            }
        }
    }


public:
    ~ConnectedComponentsAlgorithm() override = default;

    SharedPtr<MutableArraySequence<MutableArraySequence<IVertex*>>> execute(
        const IGraph<Weight>* graph,
        IVertex* startVertex = nullptr,
        IVertex* endVertex = nullptr
    ) const override {
        auto vertices = graph->getVertices();
        HashTableDictionary<size_t, bool> visited;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }

        auto components = MakeShared<MutableArraySequence<MutableArraySequence<IVertex*>>>();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex* vertex = vertices.get(i);
            if (!visited.get(vertex->getId())) {
                auto component = MakeShared<MutableArraySequence<IVertex*>>();
                dfs(vertex, graph, visited, *component);
                components->append(*component);
            }
        }

        return components;
    }
};

#endif // CONNECTEDCOMPONENTSALGORITHM_H