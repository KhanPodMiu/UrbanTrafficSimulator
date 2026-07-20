#include "simulation/RouteOptimizer.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include "simulation/RouteRequest.hpp"

RouteOptimizer::RouteOptimizer(RoutingManager* manager, int threshold)
    : routingManager(manager), congestionThreshold(threshold) {}

bool RouteOptimizer::isRouteDegraded(const Graph& graph, const RouteResult& currentRoute, const std::string& currentLocation) const {
    if (!currentRoute.isSuccess || currentRoute.intersectionIDs.empty()) return false;
    
    bool locationFound = false;
    for (size_t i = 0; i < currentRoute.intersectionIDs.size() - 1; ++i) {
        if (currentRoute.intersectionIDs[i] == currentLocation) {
            locationFound = true;
        }
        if (locationFound) {
            const std::string& from = currentRoute.intersectionIDs[i];
            const std::string& to   = currentRoute.intersectionIDs[i + 1];
            
            const auto& connectedRoads = graph.getConnectedRoads(from);
            for (const auto& road : connectedRoads) {
                if (!road) continue;
                const Intersection* dest = road->getDestinationIntersection();
                if (dest && dest->getIntersectionID() == to) {
                    if (road->getCongestionLevel() > congestionThreshold) {
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return false;
}

RouteResult RouteOptimizer::optimizeRoute(const Graph& graph, const RouteResult& currentRoute, const std::string& currentLocation, const std::string& destination) {
    if (!routingManager) {
        return currentRoute;
    }

    if (isRouteDegraded(graph, currentRoute, currentLocation)) {
        RouteRequest request(currentLocation, destination);
        RouteResult newRoute = routingManager->calculateRoute(graph, request);

        // Only accept the new route if it is actually valid
        if (newRoute.isSuccess) {
            return newRoute;
        }
    }
    return currentRoute;
}
