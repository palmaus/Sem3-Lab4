#ifndef TOPOLOGICALSORTALGORITHM_H
#define TOPOLOGICALSORTALGORITHM_H

#include "DirectedGraph.h"
#include <stdexcept>

template <typename TWeight, typename TIdentifier>
class TopologicalSortAlgorithm : public IAlgorithm<TWeight, MutableArraySequence<IVertex<TWeight, TIdentifier>*>, TIdentifier> {
private:
    void fillOrder(IVertex<TWeight, TIdentifier>* vertex, const IGraph<TWeight, TIdentifier>* graph, HashTableDictionary<TIdentifier, bool>& visited, MutableArraySequence<IVertex<TWeight, TIdentifier>*>& stack) const {
        visited.add(vertex->getId(), true);
        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* neighbor = edges.get(i)->getTo();
            if (!visited.get(neighbor->getId())) {
                fillOrder(neighbor, graph, visited, stack);
            }
        }
        stack.append(vertex);
    }


    bool hasCycleUtil(const IGraph<TWeight, TIdentifier>* graph, IVertex<TWeight, TIdentifier>* vertex, HashTableDictionary<TIdentifier, bool>& visited, HashTableDictionary<TIdentifier, bool>& recursionStack) const {
        visited.add(vertex->getId(), true);
        recursionStack.add(vertex->getId(), true);

        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex<TWeight, TIdentifier>* neighbor = edges.get(i)->getTo();
            if (!visited.get(neighbor->getId())) {
                if (hasCycleUtil(graph, neighbor, visited, recursionStack)) {
                    return true;
                }
            } else if (recursionStack.get(neighbor->getId())) {
                return true;
            }
        }

        recursionStack.add(vertex->getId(), false);
        return false;
    }


    bool hasCycle(const IGraph<TWeight, TIdentifier>* graph) const {
        const DirectedGraph<TWeight, TIdentifier>* directedGraph = dynamic_cast<const DirectedGraph<TWeight, TIdentifier>*>(graph);
        if (!directedGraph) {
            return false;
        }

        HashTableDictionary<TIdentifier, bool> visited;
        HashTableDictionary<TIdentifier, bool> recursionStack;
        auto vertices = graph->getVertices();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
            recursionStack.add(vertices.get(i)->getId(), false);
        }

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            if (!visited.get(vertices.get(i)->getId())) {
                if (hasCycleUtil(graph, vertices.get(i), visited, recursionStack)) {
                    return true;
                }
            }
        }

        return false;
    }


public:
    ~TopologicalSortAlgorithm() override = default;

    SharedPtr<MutableArraySequence<IVertex<TWeight, TIdentifier>*>> execute(
        const IGraph<TWeight, TIdentifier>* graph,
        IVertex<TWeight, TIdentifier>* startVertex = nullptr,
        IVertex<TWeight, TIdentifier>* endVertex = nullptr
    ) const override {

        const DirectedGraph<TWeight, TIdentifier>* directedGraph = dynamic_cast<const DirectedGraph<TWeight, TIdentifier>*>(graph);
        if (!directedGraph) {
            throw std::runtime_error("Topological sort can be applied to directed graphs only");
        }

        if (hasCycle(graph)) {
            throw std::runtime_error("Topological sort is not defined for cyclic graphs.");
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

        auto result = MakeShared<MutableArraySequence<IVertex<TWeight, TIdentifier>*>>();
        for (int i = stack->getLength() - 1; i >= 0; --i) {
            result->append(stack->get(i));
        }
        return result;
    }
};

#endif // TOPOLOGICALSORTALGORITHM_H