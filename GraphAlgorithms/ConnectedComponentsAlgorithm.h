#ifndef CONNECTEDCOMPONENTSALGORITHM_H
#define CONNECTEDCOMPONENTSALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include "SharedPtr.h"
#include <functional>

template <typename TWeight, typename TIdentifier>
class ConnectedComponentsAlgorithm : public IAlgorithm<TWeight, MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>, TIdentifier> {
private:
    void dfs(IVertex<TIdentifier>* vertex, const IGraph<TWeight, TIdentifier>* graph, HashTableDictionary<TIdentifier, bool>& visited, MutableArraySequence<IVertex<TIdentifier>*>& component) const {
        visited.add(vertex->getId(), true);
        component.append(vertex);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TIdentifier>* neighbor = nullptr;
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

    SharedPtr<MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>> execute(
        const IGraph<TWeight, TIdentifier>* graph,
        IVertex<TIdentifier>* startVertex = nullptr,
        IVertex<TIdentifier>* endVertex = nullptr
    ) const override {
        auto vertices = graph->getVertices();
        HashTableDictionary<TIdentifier, bool> visited;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }

        auto components = MakeShared<MutableArraySequence<MutableArraySequence<IVertex<TIdentifier>*>>>();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex<TIdentifier>* vertex = vertices.get(i);
            if (!visited.get(vertex->getId())) {
                auto component = MakeShared<MutableArraySequence<IVertex<TIdentifier>*>>();
                dfs(vertex, graph, visited, *component);
                components->append(*component);
            }
        }

        return components;
    }
};

#endif