#ifndef DIJKSTRAALGORITHM_H
#define DIJKSTRAALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "PriorityQueue.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include <limits>
#include <stdexcept>

template <typename Weight>
class DijkstraAlgorithm : public IAlgorithm<Weight, MutableArraySequence<Weight>> {
public:
    ~DijkstraAlgorithm() override = default;

    SharedPtr<MutableArraySequence<Weight>> execute(
        const IGraph<Weight>* graph,
        IVertex* startVertex = nullptr,
        IVertex* endVertex = nullptr
    ) const override {

        if (!startVertex) {
            throw std::invalid_argument("Start vertex is not specified.");
        }

        if (!graph->hasVertex(startVertex)) {
            throw std::invalid_argument("Start vertex does not exist in the graph.");
        }

        auto vertices = graph->getVertices();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            auto edges = graph->getEdges(vertices.get(i));
            for (size_t j = 0; j < edges.getLength(); ++j) {
                if (edges.get(j)->getWeight() < 0) {
                    throw std::runtime_error("Dijkstra's algorithm does not support negative edge weights.");
                }
            }
        }

        HashTableDictionary<size_t, Weight> distances;
        PriorityQueue<size_t, Weight> queue;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            distances.add(vertices.get(i)->getId(), std::numeric_limits<Weight>::max());
        }

        distances.add(startVertex->getId(), 0);
        queue.enqueue(startVertex->getId(), 0);

        while (!queue.isEmpty()) {
            size_t currentVertexId = queue.dequeue();
            IVertex* currentVertex = graph->getVertexById(currentVertexId);
            if (!currentVertex) continue;

            auto edges = graph->getEdges(currentVertex);

            for (size_t i = 0; i < edges.getLength(); ++i) {
                IVertex* neighborVertex = nullptr;
                if (edges.get(i)->getFrom()->getId() == currentVertexId) {
                    neighborVertex = edges.get(i)->getTo();
                } else {
                    neighborVertex = edges.get(i)->getFrom();
                }
                if (!neighborVertex) continue;

                Weight weight = edges.get(i)->getWeight();

                Weight newDistance = distances.get(currentVertexId) + weight;


                if (newDistance < distances.get(neighborVertex->getId())) {
                    distances.add(neighborVertex->getId(), newDistance);
                    queue.enqueue(neighborVertex->getId(), newDistance); // Обновляем приоритет

                } else {
                }
            }
        }

        auto result = MakeShared<MutableArraySequence<Weight>>();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            result->append(distances.get(vertices.get(i)->getId()));
        }
        return result;
    }
};

#endif // DIJKSTRAALGORITHM_H