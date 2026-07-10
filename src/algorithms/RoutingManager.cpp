#include "algorithms/RoutingManager.hpp"

RoutingManager::RoutingManager(std::unique_ptr<PathFindingStrategy> initialStrategy)
    : strategy(std::move(initialStrategy)) {}

void RoutingManager::setStrategy(std::unique_ptr<PathFindingStrategy> newStrategy) {
    strategy = std::move(newStrategy);
}

RouteResult RoutingManager::calculateRoute(const Graph& graph, const RouteRequest& request) const {
    if (!strategy) {
        return RouteResult();
    }
    return strategy->calculateRoute(graph, request);
}
