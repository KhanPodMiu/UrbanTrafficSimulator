#include "utils/event_handling.hpp"
#include "utils/vector2i.hpp"
#include "SDL2/SDL_image.h"
#include "graph/Intersection.hpp" 
#include "graph/Road.hpp"
#include "simulation/VehicleManager.hpp"
#include "simulation/RouteOptimizer.hpp"
#include "simulation/PresidentialRouteManager.hpp"
#include "core/Constants.hpp"
#include <iostream>
#include <filesystem>
#include "utils/MapLoader.hpp"
#include "visualization/VisualizationEngine.hpp"

// Loads a banned-route overlay (or the default map) based on the given routeID.
// routeID 1..6 select pre-defined banned-route JSON files;
// routeID 0 (or any other value) reverts to the original map.
bool switchBannedRoute(int routeID, AppContext &Game) 
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

    std::filesystem::path mapPath = resolveAssetPath(filePath);
    
    if (MapLoader::loadFromJson(mapPath.string(), Game.graph)) {
        Game.isBannedState = (routeID != 0);
        /*
        // Legacy behavior (disabled): this made the closure active and
        // rerouted vehicles in a single step, leaving no clearance window.
        const int reroutedVehicles =
            Game.vehicleManager.rerouteVehiclesAroundBannedRoads();
        */
        int reroutedVehicles = 0;
        if (routeID != 0)
        {
            reroutedVehicles =
                Game.presidentialRouteManager.beginPreparation();
        }
        else
        {
            Game.presidentialRouteManager.cancel();
        }
        Game.visualizationEngine.buildRenderCache(Game.graph);
        std::cout << "[INFO] Successfully loaded route ID: " << routeID 
                  << " (" << filePath << "), rerouted vehicles: "
                  << reroutedVehicles;
        if (routeID != 0)
        {
            std::cout
                << ", waiting until the corridor is empty and no vehicle "
                << "route points into it";
        }
        std::cout << "\n";
        return true;
    }

    std::cerr << "[ERROR] Failed to load route ID: " << routeID 
              << " from path: " << filePath << "\n";
    return false;
}

void handleInput(SDL_Event& event, AppContext &Game)
{
    g_inputHandler.handleInput(event, Game);
}

bool wasDragAction()
{
    return g_inputHandler.wasDragAction();
}

void spawnVehicleAt(const Vector2& clickPos, Graph& graph, VehicleManager& vehicleManager)
{
    g_inputHandler.spawnVehicleAt(clickPos, graph, vehicleManager);
}

bool switchBannedRoute(int routeID, AppContext &Game)
{
    return g_inputHandler.switchBannedRoute(routeID, Game);
}
