#pragma once
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>

#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

enum HeuristicType {
    Zero,
    Euclidean,
    WeightedEuclidean,
    DivideSpeed
};

class AStarStrategy
{
public:
    explicit AStarStrategy(
        HeuristicType heuristic =
            HeuristicType::Euclidean);

    RouteResult calculateRoute(const Graph& graph, const RouteRequest& request);
    std::size_t getExpandedNodeCount() const {
        return expandedNodes;
    }

     double getLastTravelCost() const
    {
        return lastTravelCost;
    }

private:

    struct Node
    {
        std::string intersectionID;
        double g;
        double h;
        double f;
    };
    double lastTravelCost = 0.0;
    struct Compare
    {
        bool operator()(const Node& lhs, const Node& rhs) const {
            // Smaller f has higher priority.
            if (lhs.f == rhs.f)
                return lhs.h > rhs.h;     // tie-breaker
            return lhs.f > rhs.f;
        }
    };
    HeuristicType heuristicType;
    double heuristic(const Intersection& current, const Intersection& goal) const;
    RouteResult reconstructPath(
        const std::unordered_map<std::string,std::string>& cameFrom,
        const std::string& start,
        const std::string& goal) const;
    std::size_t expandedNodes = 0;
};

