#pragma once
#include <memory>
#include "algorithms/PathFindingStrategy.hpp"
#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class RoutingManager {
private:
    std::unique_ptr<PathFindingStrategy> strategy;
public:
    RoutingManager(std::unique_ptr<PathFindingStrategy> initialStrategy = nullptr);
    void setStrategy(std::unique_ptr<PathFindingStrategy> newStrategy);
    RouteResult calculateRoute(const Graph& graph, const RouteRequest& request) const;
};
