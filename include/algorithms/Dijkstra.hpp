#pragma once 

#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class Dijkstra {
public:
    static RouteResult findShortestPath(const Graph& graph, const RouteRequest& request);
};
