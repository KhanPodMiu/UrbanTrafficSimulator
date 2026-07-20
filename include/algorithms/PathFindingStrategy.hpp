#pragma once

#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class PathFindingStrategy {
public:
    virtual ~PathFindingStrategy() = default;
    virtual RouteResult calculateRoute(const Graph& graph, const RouteRequest& request) const = 0;
};
