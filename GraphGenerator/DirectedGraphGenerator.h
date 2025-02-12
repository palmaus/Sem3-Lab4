#ifndef DIRECTEDGRAPHGENERATOR_H
#define DIRECTEDGRAPHGENERATOR_H

#include "IGraphGenerator.h"
#include "DirectedGraph.h"
#include <random>

template <typename Weight, typename TIdentifier>
class DirectedGraphGenerator : public IGraphGenerator<Weight, TIdentifier> {
private:
    size_t numVertices_;
    double edgeProbability_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dist_;

public:
    DirectedGraphGenerator(size_t numVertices, double edgeProbability)
      : numVertices_(numVertices), edgeProbability_(edgeProbability),
        gen_(std::random_device{}()), dist_(0.0, 1.0)
    {
        if (numVertices_ == 0) {
            throw std::invalid_argument("Number of vertices must be greater than 0.");
        }
        if (edgeProbability_ < 0.0 || edgeProbability_ > 1.0)
        {
            throw std::invalid_argument("Probability must be within 0 and 1 inclusive");
        }
    }

    IGraph<Weight, TIdentifier>* generate() override {
        auto graph = new DirectedGraph<Weight, TIdentifier>();

        for (size_t i = 0; i < numVertices_; ++i) {
            graph->addVertex(new Vertex<TIdentifier>(static_cast<TIdentifier>(i)));
        }

        auto vertices = graph->getVertices();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            for (size_t j = 0; j < vertices.getLength(); ++j) {
                if (i != j) {
                    if (dist_(gen_) < edgeProbability_) {
                        std::uniform_int_distribution<Weight> weightDist(1, 10);
                        Weight weight = weightDist(gen_);
                        graph->addEdge(vertices.get(i), vertices.get(j), weight);
                    }
                }
            }
        }

        return graph;
    }
};

#endif // DIRECTEDGRAPHGENERATOR_H