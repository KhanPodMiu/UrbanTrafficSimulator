#include "simulation/BannedRouteManager.hpp"
#include "graph/Graph.hpp"
#include "simulation/VehicleManager.hpp"
#include "visualization/VisualizationEngine.hpp"
#include "utils/MapLoader.hpp"
#include "utils/event_handling.hpp"
#include <iostream>

bool BannedRouteManager::switchBannedRoute(
    int routeID,
    Graph& graph,
    VehicleManager& vehicleManager,
    VisualizationEngine& visualizationEngine,
    bool& isBannedState)
{
    std::string filePath;

    switch (routeID) {
        case 1:
            filePath = "assets/bannedRoutes/khapkhap_routeBanned_01.json";
            break;
        case 2:
            filePath = "assets/bannedRoutes/khapkhap_routeBanned_02.json";
            break;
        case 3:
            filePath = "assets/bannedRoutes/khapkhap_routeBanned_03.json";
            break;
        case 4:
            filePath = "assets/bannedRoutes/khapkhap_routeBanned_04.json";
            break;
        case 5:
            filePath = "assets/bannedRoutes/khapkhap_routeBanned_05.json";
            break;
        case 6:
            filePath = "assets/bannedRoutes/khapkhap_routeBanned.json";
            break;
        default:
            filePath = "assets/maps/khapkhap.json";
            break;
    }

    std::filesystem::path mapPath = ::resolveAssetPath(filePath);

    if (MapLoader::loadFromJson(mapPath.string(), graph)) {
        isBannedState = (routeID != 0);
        const int reroutedVehicles =
            vehicleManager.rerouteVehiclesAroundBannedRoads();
        visualizationEngine.buildRenderCache(graph);
        std::cout << "[INFO] Successfully loaded route ID: " << routeID 
                  << " (" << filePath << "), rerouted vehicles: "
                  << reroutedVehicles << "\n";
        return true;
    }

    std::cerr << "[ERROR] Failed to load route ID: " << routeID 
              << " from path: " << filePath << "\n";
    return false;
}
