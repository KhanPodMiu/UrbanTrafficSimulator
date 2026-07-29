#include "utils/event_handling.hpp"
#include "utils/vector2i.hpp"
#include "SDL2/SDL_image.h"
#include "graph/Intersection.hpp" 
#include "graph/Road.hpp"
#include "simulation/VehicleManager.hpp"
#include "simulation/RouteOptimizer.hpp"
#include "utils/vector2i.hpp"
#include "core/Constants.hpp"
#include <iostream>
#include <filesystem>
#include "utils/MapLoader.hpp"                    
#include "visualization/VisualizationEngine.hpp"

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
            filePath = "assets/maps/khapkhap.json"; // Trở về map gốc (không cấm đường nào)
            break;
    }

    std::filesystem::path mapPath = resolveAssetPath(filePath);
    
    if (MapLoader::loadFromJson(mapPath.string(), Game.graph)) {
        Game.isBannedState = (routeID != 0);
        Game.visualizationEngine.buildRenderCache(Game.graph);
        std::cout << "[INFO] Successfully loaded route ID: " << routeID 
                  << " (" << filePath << ")\n";
        return true;
    }

    std::cerr << "[ERROR] Failed to load route ID: " << routeID 
              << " from path: " << filePath << "\n";
    return false;
}

void handleInput(SDL_Event& event, AppContext &Game)
{

    if (event.type == SDL_QUIT)
    {
        Game.isRunning = false;
        return;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        float screenX = static_cast<float>(event.button.x) - Config::PANEL_WIDTH; 
        float screenY = static_cast<float>(event.button.y);

        if (screenX < 0) return;

        Vector2 worldPos;
        worldPos.x = Game.camera.getX() + (screenX / Game.camera.getZoom());
        worldPos.y = Game.camera.getY() + (screenY / Game.camera.getZoom());
        
        spawnVehicleAt(worldPos, Game.graph, Game.vehicleManager);
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                Game.isRunning = false;
                break;

            case SDLK_1: switchBannedRoute(1, Game); break;
            case SDLK_2: switchBannedRoute(2, Game); break;
            case SDLK_3: switchBannedRoute(3, Game); break;
            case SDLK_4: switchBannedRoute(4, Game); break;
            case SDLK_5: switchBannedRoute(5, Game); break;
            case SDLK_6: switchBannedRoute(6, Game); break;

            case SDLK_0: switchBannedRoute(0, Game); break;

            case SDLK_b: {
                int nextRoute = Game.isBannedState ? 0 : 6;
                switchBannedRoute(nextRoute, Game);
                break;
            }

            case SDLK_w: 
                Game.camera.subY(); 
                break;

            case SDLK_a: 
                Game.camera.subX(); 
                break;

            case SDLK_s: 
                Game.camera.addY(); 
                break;

            case SDLK_d: 
                Game.camera.addX(); 
                break;

            case SDLK_q: 
                Game.camera.zoomOut(); 
                break;

            case SDLK_e: 
                Game.camera.zoomIn(); 
                break;
        }
    }
}

void spawnVehicleAt(const Vector2& clickPos, 
                    Graph& graph, 
                    VehicleManager& vehicleManager)
                     
{
    Vector2 mousePos(static_cast<float>(clickPos.x), static_cast<float>(clickPos.y));

    auto startNode = graph.findNearestIntersection(mousePos, 300.0f);
    if (startNode) 
    {
        /* std::cout << "[DEBUG] Found intersection: " << startNode->getIntersectionID() << std::endl; */
        vehicleManager.spawnVehicleAtIntersection(startNode->getIntersectionID());
        return;
    }
    else
    {
        /* std::cout << "[DEBUG] Missed click, checking for nearest road..." << std::endl; */
    }

    auto clickedRoad = graph.findNearestRoad(mousePos, 100.0f);
    if (clickedRoad)
    {
        const Intersection* nextIntersection = clickedRoad->getDestinationIntersection();
        if (nextIntersection) 
        {
            vehicleManager.spawnVehicleAtIntersection(nextIntersection->getIntersectionID());
        }
    }
}

