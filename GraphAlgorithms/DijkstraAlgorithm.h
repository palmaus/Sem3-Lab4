#ifndef DIJKSTRAALGORITHM_H
#define DIJKSTRAALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "PriorityQueue.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include "GraphPath.h"
#include <limits>
#include <stdexcept>
#include <iostream>
#include <utility>

template <typename Weight, typename TIdentifier>
class DijkstraAlgorithm : public IAlgorithm<Weight, std::pair<MutableArraySequence<Weight>, GraphPath<Weight, TIdentifier>>, TIdentifier> {
public:
    using DistanceSequence = MutableArraySequence<Weight>;
    using PathResult = std::pair<DistanceSequence, GraphPath<Weight, TIdentifier>>;
    using VertexPtr = IVertex<TIdentifier>*;
    using VertexSequence = MutableArraySequence<VertexPtr>;
    using PredecessorMap = HashTableDictionary<TIdentifier, VertexPtr>;

    ~DijkstraAlgorithm() override = default;

    SharedPtr<PathResult> execute(
        const IGraph<Weight, TIdentifier>* graph,
        VertexPtr startVertex = nullptr,
        VertexPtr endVertex = nullptr
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

        HashTableDictionary<TIdentifier, Weight> distances;
        PriorityQueue<TIdentifier, Weight> queue;
        PredecessorMap predecessors;

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            distances.add(vertices.get(i)->getId(), std::numeric_limits<Weight>::max());
            predecessors.add(vertices.get(i)->getId(), nullptr);
        }

        distances.add(startVertex->getId(), 0);
        queue.enqueue(startVertex->getId(), 0);


        while (!queue.isEmpty()) {
            TIdentifier currentVertexId = queue.dequeue();
            VertexPtr currentVertex = graph->getVertexById(currentVertexId);

            if (!currentVertex) {
                continue;
            }

            auto edges = graph->getEdges(currentVertex);

            for (size_t i = 0; i < edges.getLength(); ++i) {
                VertexPtr neighborVertex = edges.get(i)->getTo();
                if (!neighborVertex) {
                    continue;
                }

                Weight weight = edges.get(i)->getWeight();
                Weight newDistance = distances.get(currentVertexId) + weight;

                if (newDistance < distances.get(neighborVertex->getId())) {
                    distances.add(neighborVertex->getId(), newDistance);
                    predecessors.add(neighborVertex->getId(), currentVertex);
                    queue.enqueue(neighborVertex->getId(), newDistance);

                }
            }
        }

        auto distanceResult = MakeShared<DistanceSequence>();
        auto pathVertices = MakeShared<VertexSequence>();

        if (endVertex && distances.get(endVertex->getId()) != std::numeric_limits<Weight>::max()) {
            VertexPtr current = endVertex;
            while (current != nullptr) {
                pathVertices->prepend(current);
                current = predecessors.get(current->getId());
            }
        }


        for (size_t i = 0; i < vertices.getLength(); ++i) {
            distanceResult->append(distances.get(vertices.get(i)->getId()));
        }

        GraphPath<Weight, TIdentifier> path(pathVertices);
        return MakeShared<PathResult>(std::make_pair(*distanceResult, path));
    }
};

#endif // DIJKSTRAALGORITHM_H