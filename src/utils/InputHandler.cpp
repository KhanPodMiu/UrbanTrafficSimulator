#include "utils/InputHandler.hpp"
#include "core/Constants.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "simulation/VehicleManager.hpp"
#include "visualization/VisualizationEngine.hpp"
#include <iostream>

void InputHandler::handleInput(SDL_Event& event, AppContext& Game)
{
    if (event.type == SDL_QUIT)
    {
        Game.isRunning = false;
        return;
    }

    if (event.type == SDL_MOUSEWHEEL)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        const float viewportX = static_cast<float>(mouseX) - Config::PANEL_WIDTH;
        const float viewportY = static_cast<float>(mouseY);

        if (viewportX >= 0)
        {
            if (event.wheel.y > 0)
                Game.camera.zoomIn(viewportX, viewportY);
            else if (event.wheel.y < 0)
                Game.camera.zoomOut(viewportX, viewportY);
        }

        return;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            const float screenX = static_cast<float>(event.button.x) - Config::PANEL_WIDTH;

            if (screenX >= 0)
            {
                isDragging_ = true;
                lastMouseX_ = event.button.x;
                lastMouseY_ = event.button.y;
                dragStartX_ = event.button.x;
                dragStartY_ = event.button.y;
                mouseMoved_ = false;
            }
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging_ = true;
            lastMouseX_ = event.button.x;
            lastMouseY_ = event.button.y;
            dragStartX_ = event.button.x;
            dragStartY_ = event.button.y;
            mouseMoved_ = false;
        }

        return;
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_LEFT ||
            event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging_ = false;
        }

        return;
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        if (isDragging_)
        {
            int dx = event.motion.x - lastMouseX_;
            int dy = event.motion.y - lastMouseY_;

            Game.camera.offsetPosition(
                -static_cast<float>(dx) / Game.camera.getZoom(),
                -static_cast<float>(dy) / Game.camera.getZoom());

            lastMouseX_ = event.motion.x;
            lastMouseY_ = event.motion.y;

            if (!mouseMoved_)
            {
                int totalDx = event.motion.x - dragStartX_;
                int totalDy = event.motion.y - dragStartY_;
                if (totalDx * totalDx + totalDy * totalDy >
                    DRAG_THRESHOLD * DRAG_THRESHOLD)
                {
                    mouseMoved_ = true;
                }
            }
        }

        return;
    }

    if (event.type != SDL_KEYDOWN)
        return;

    switch (event.key.keysym.sym)
    {
        case SDLK_ESCAPE:
            Game.isRunning = false;
            break;

        case SDLK_v: {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            float screenX = static_cast<float>(mouseX) - Config::PANEL_WIDTH;
            float screenY = static_cast<float>(mouseY);
            if (screenX >= 0) {
                Vector2 worldPos;
                worldPos.x = Game.camera.getX() + (screenX / Game.camera.getZoom());
                worldPos.y = Game.camera.getY() + (screenY / Game.camera.getZoom());

                spawnVehicleAt(worldPos, Game.graph, Game.vehicleManager);
            }
            break;
        }

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

bool InputHandler::switchBannedRoute(int routeID, AppContext& Game)
{
    return bannedRouteManager_.switchBannedRoute(
        routeID,
        Game.graph,
        Game.vehicleManager,
        Game.visualizationEngine,
        Game.isBannedState);
}

void InputHandler::spawnVehicleAt(
    const Vector2& clickPos, 
    Graph& graph, 
    VehicleManager& vehicleManager)
{
    Vector2 mousePos(static_cast<float>(clickPos.x), static_cast<float>(clickPos.y));

    auto startNode = graph.findNearestIntersection(mousePos, 200.0f);
    if (startNode) 
    {
        vehicleManager.spawnVehicleAtIntersection(startNode->getIntersectionID());
        return;
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
