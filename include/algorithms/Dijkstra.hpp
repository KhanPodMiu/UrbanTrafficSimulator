#pragma once 

#include "Graph.hpp"
#include "RouteRequest.hpp"
#include "RouteResult.hpp"

class Dijkstra {
public:
    static RouteResult findShortestPath(const Graph& graph, const RouteRequest& request);
};
