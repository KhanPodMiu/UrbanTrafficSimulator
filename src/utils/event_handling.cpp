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

void handleInput(SDL_Event& event, bool& isRunning, Camera& camera, Graph& graph, VehicleManager& vehicleManager)
{
 
    if (event.type == SDL_QUIT)
    {
        isRunning = false;
        return;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        float screenX = static_cast<float>(event.button.x) - Config::PANEL_WIDTH; 
        float screenY = static_cast<float>(event.button.y);

        if (screenX < 0) return; 

        Vector2 worldPos;
        worldPos.x = camera.getX() + (screenX / camera.getZoom());
        worldPos.y = camera.getY() + (screenY / camera.getZoom());
     
        spawnVehicleAt(worldPos, graph, vehicleManager);
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                isRunning = false;
                break;
                
            case SDLK_w: 
                camera.subY(); 
                break;

            case SDLK_a: 
                camera.subX(); 
                break;

            case SDLK_s: 
                camera.addY(); 
                break;

            case SDLK_d: 
                camera.addX(); 
                break;

            case SDLK_q: 
                camera.zoomOut(); 
                break;

            case SDLK_e: 
                camera.zoomIn(); 
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

