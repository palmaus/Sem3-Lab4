#ifndef TOPOLOGICALSORTALGORITHM_H
#define TOPOLOGICALSORTALGORITHM_H

#include "DirectedGraph.h"
#include <stdexcept>

template <typename Weight>
class TopologicalSortAlgorithm : public IAlgorithm<Weight, MutableArraySequence<IVertex*>> {
private:
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


    bool hasCycleUtil(const IGraph<Weight>* graph, IVertex* vertex, HashTableDictionary<size_t, bool>& visited, HashTableDictionary<size_t, bool>& recursionStack) const {
        visited.add(vertex->getId(), true);
        recursionStack.add(vertex->getId(), true);

        auto edges = graph->getEdges(vertex);
        for (size_t i = 0; i < edges.getLength(); ++i) {
            IVertex* neighbor = edges.get(i)->getTo();
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


    bool hasCycle(const IGraph<Weight>* graph) const {
        const DirectedGraph<Weight>* directedGraph = dynamic_cast<const DirectedGraph<Weight>*>(graph);
        if (!directedGraph) {
            return false;
        }

        HashTableDictionary<size_t, bool> visited;
        HashTableDictionary<size_t, bool> recursionStack;
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

    SharedPtr<MutableArraySequence<IVertex*>> execute(
        const IGraph<Weight>* graph,
        IVertex* startVertex = nullptr,
        IVertex* endVertex = nullptr
    ) const override {

        const DirectedGraph<Weight>* directedGraph = dynamic_cast<const DirectedGraph<Weight>*>(graph);
        if (!directedGraph) {
            throw std::runtime_error("Topological sort can be applied to directed graphs only");
        }

        if (hasCycle(graph)) {
            throw std::runtime_error("Topological sort is not defined for cyclic graphs.");
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

        auto result = MakeShared<MutableArraySequence<IVertex*>>();
        for (int i = stack->getLength() - 1; i >= 0; --i) {
            result->append(stack->get(i));
        }
        return result;
    }
};

#endif // TOPOLOGICALSORTALGORITHM_H