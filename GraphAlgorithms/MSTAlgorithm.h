#ifndef MSTALGORITHM_H
#define MSTALGORITHM_H

#include "IAlgorithm.h"
#include "IGraph.h"
#include "MutableArraySequence.h"
#include "PriorityQueue.h"
#include "HashTableDictionary.h"
#include "IVertex.h"
#include "IEdge.h"
#include "SharedPtr.h"
#include <limits>
#include <stdexcept>
#include <functional>

template <typename Weight>
class MSTAlgorithm : public IAlgorithm<Weight, MutableArraySequence<IEdge<Weight>*>> {
private:
    bool isGraphConnected(const IGraph<Weight>* graph) const {
        auto vertices = graph->getVertices();
        if (vertices.getLength() <= 1) return true;

        HashTableDictionary<size_t, bool> visited;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            visited.add(vertices.get(i)->getId(), false);
        }


        std::function<void(IVertex*)> dfs = [&](IVertex* vertex) {
            visited.add(vertex->getId(), true);
            auto edges = graph->getEdges(vertex);
            for (size_t i = 0; i < edges.getLength(); ++i) {
                IVertex* neighbor = nullptr;
                if (edges.get(i)->getFrom()->getId() == vertex->getId()) {
                    neighbor = edges.get(i)->getTo();
                } else {
                    neighbor = edges.get(i)->getFrom();
                }
                if (!visited.get(neighbor->getId())) {
                    dfs(neighbor);
                }
            }
        };

        dfs(vertices.get(0));

        for (size_t i = 0; i < vertices.getLength(); ++i) {
            if (!visited.get(vertices.get(i)->getId())) {
                return false;
            }
        }
        return true;
    }
public:
    ~MSTAlgorithm() override = default;

    SharedPtr<MutableArraySequence<IEdge<Weight>*>> execute(
        const IGraph<Weight>* graph,
        IVertex* startVertex = nullptr,
        IVertex* endVertex = nullptr
    ) const override {
        auto vertices = graph->getVertices();
        if (vertices.getLength() == 0) {
            return MakeShared<MutableArraySequence<IEdge<Weight>*>>();
        }
        if (vertices.getLength() > 1) {
            if (!isGraphConnected(graph)) {
                throw std::runtime_error("Graph is not connected, MST does not exist.");
            }
        }
        HashTableDictionary<size_t, bool> inMST;
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            inMST.add(vertices.get(i)->getId(), false);
        }

        PriorityQueue<std::pair<size_t, size_t>, Weight> edgesPQ;
        auto mstEdges = MakeShared<MutableArraySequence<IEdge<Weight>*>>();

        IVertex* startVertex_ = vertices.get(0);
        inMST.add(startVertex_->getId(), true);
        auto initialEdges = graph->getEdges(startVertex_);
        for (size_t i = 0; i < initialEdges.getLength(); ++i) {
            IVertex* neighborVertex = nullptr;
            if (initialEdges.get(i)->getFrom()->getId() == startVertex_->getId()) {
                neighborVertex = initialEdges.get(i)->getTo();
            } else {
                neighborVertex = initialEdges.get(i)->getFrom();
            }
            edgesPQ.enqueue({startVertex_->getId(), neighborVertex->getId()}, initialEdges.get(i)->getWeight());
        }

        while (mstEdges->getLength() < vertices.getLength() - 1 && !edgesPQ.isEmpty()) {
            auto [fromVertexId, toVertexId] = edgesPQ.dequeue();
            if (inMST.get(toVertexId)) {
                continue;
            }
            IVertex* fromV = graph->getVertexById(fromVertexId);
            IVertex* toV = graph->getVertexById(toVertexId);
            if (!fromV || !toV) continue;

            IEdge<Weight>* edge = nullptr;
            auto currentEdges = graph->getEdges(fromV);
            for (size_t i = 0; i < currentEdges.getLength(); ++i) {
                IVertex* neighbor = nullptr;
                if (currentEdges.get(i)->getFrom()->getId() == fromV->getId()) {
                    neighbor = currentEdges.get(i)->getTo();
                } else {
                    neighbor = currentEdges.get(i)->getFrom();
                }
                if (neighbor->getId() == toVertexId) {
                    edge = currentEdges.get(i);
                    break;
                }
            }
            if (!edge) continue;

            mstEdges->append(edge);
            inMST.add(toVertexId, true);

            auto newEdges = graph->getEdges(toV);
            for (size_t i = 0; i < newEdges.getLength(); ++i) {
                IVertex* neighborVertex = nullptr;
                if (newEdges.get(i)->getFrom()->getId() == toV->getId()) {
                    neighborVertex = newEdges.get(i)->getTo();
                } else {
                    neighborVertex = newEdges.get(i)->getFrom();
                }
                Weight weight = newEdges.get(i)->getWeight();
                if (!inMST.get(neighborVertex->getId())) {
                    edgesPQ.enqueue({toVertexId, neighborVertex->getId()}, weight);
                }
            }
        }
        return mstEdges;
    }
};

#endif // MSTALGORITHM_H