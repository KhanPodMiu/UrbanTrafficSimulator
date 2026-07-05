#pragma once

#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class BFS {
public:
    static RouteResult findShortestPath(const Graph& graph, const RouteRequest& request);

static size_t getExpandedNodeCount();

private:
    static size_t expandedNodeCount;
};
