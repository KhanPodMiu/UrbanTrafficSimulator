#pragma once

#include <filesystem>
#include <string>

class Graph;
class PresidentialRouteManager;
class VehicleManager;
class VisualizationEngine;

class BannedRouteManager
{
public:
    BannedRouteManager() = default;
    ~BannedRouteManager() = default;

    bool switchBannedRoute(
        int routeID,
        Graph& graph,
        VehicleManager& vehicleManager,
        PresidentialRouteManager& presidentialRouteManager,
        VisualizationEngine& visualizationEngine,
        bool& isBannedState);
};
