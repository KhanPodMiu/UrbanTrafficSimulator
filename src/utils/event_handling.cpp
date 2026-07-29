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

            case SDLK_b: { 
                std::string mapFile = Game.isBannedState ? "assets/maps/khapkhap.json" 
                                                        : "assets/bannedRoutes/khapkhap_routeBanned.json";
                
                std::filesystem::path mapPath = resolveAssetPath(mapFile);
                if (MapLoader::loadFromJson(mapPath.string(), Game.graph)) {
                    // Đảo ngược trạng thái cờ
                    Game.isBannedState = !Game.isBannedState;
                    
                    Game.visualizationEngine.buildRenderCache(Game.graph);
                    
                    if (Game.isBannedState) {
                        //std::cout << "Da KICH HOAT tuyen duong cam!\n";
                    } else {
                       // std::cout << "Da TAT tuyen duong cam (Tro ve trang thai ban dau)!\n";
                    }
                } else {
                    std::cerr << "Loi: Khong the load file map!\n";
                }
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

