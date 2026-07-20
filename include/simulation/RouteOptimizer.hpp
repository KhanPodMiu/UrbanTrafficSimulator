#pragma once
#include <string>
#include "graph/Graph.hpp"
#include "algorithms/RoutingManager.hpp"
#include "simulation/RouteResult.hpp"

class RouteOptimizer {
private:
    RoutingManager* routingManager;
    int congestionThreshold;
public:
    RouteOptimizer(RoutingManager* manager, int threshold = 70);
    
    bool isRouteDegraded(const Graph& graph, const RouteResult& currentRoute, const std::string& currentLocation) const;
    RouteResult optimizeRoute(const Graph& graph, const RouteResult& currentRoute, const std::string& currentLocation, const std::string& destination);
};
