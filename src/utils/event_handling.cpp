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

void handleInput(SDL_Event& event, bool& isRunning, Camera& camera,
                 Graph& graph, VehicleManager& vehicleManager)
{
    if (event.type == SDL_QUIT)
    {
        isRunning = false;
        return;
    }

    static bool isDragging = false;
    static int lastMouseX = 0;
    static int lastMouseY = 0;

    // Mouse wheel -> zoom
    if (event.type == SDL_MOUSEWHEEL)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (event.wheel.y > 0)
            camera.zoomIn(static_cast<float>(mouseX), static_cast<float>(mouseY));
        else if (event.wheel.y < 0)
            camera.zoomOut(static_cast<float>(mouseX), static_cast<float>(mouseY));

        return;
    }

    // Mouse pressed
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            // Spawn vehicle
            float screenX = static_cast<float>(event.button.x) - Config::PANEL_WIDTH;
            float screenY = static_cast<float>(event.button.y);

            if (screenX >= 0)
            {
                Vector2 worldPos;
                worldPos.x = camera.getX() + screenX / camera.getZoom();
                worldPos.y = camera.getY() + screenY / camera.getZoom();

                spawnVehicleAt(worldPos, graph, vehicleManager);
            }

            // Start dragging
            isDragging = true;
            lastMouseX = event.button.x;
            lastMouseY = event.button.y;
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging = true;
            lastMouseX = event.button.x;
            lastMouseY = event.button.y;
        }

        return;
    }

    // Mouse released
    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_LEFT ||
            event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging = false;
        }

        return;
    }

    // Drag camera
    if (event.type == SDL_MOUSEMOTION)
    {
        if (isDragging)
        {
            int dx = event.motion.x - lastMouseX;
            int dy = event.motion.y - lastMouseY;

            camera.offsetPosition(
                -static_cast<float>(dx) / camera.getZoom(),
                -static_cast<float>(dy) / camera.getZoom());

            lastMouseX = event.motion.x;
            lastMouseY = event.motion.y;
        }

        return;
    }

    if (event.type != SDL_KEYDOWN)
        return;

    switch (event.key.keysym.sym)
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

