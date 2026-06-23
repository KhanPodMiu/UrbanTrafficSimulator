#pragma once

#include "Graph.hpp"
#include "RouteRequest.hpp"
#include "RouteResult.hpp"

class BFS {
public:
    static RouteResult findShortestPath(const Graph& graph, const RouteRequest& request);
};
