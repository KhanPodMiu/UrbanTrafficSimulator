#pragma once 

#include "algorithms/PathFindingStrategy.hpp"
#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class Dijkstra : public PathFindingStrategy {
public:
    RouteResult calculateRoute(const Graph& graph, const RouteRequest& request) const override;
};
