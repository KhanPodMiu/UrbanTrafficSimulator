#include "utils/event_handling.hpp"
#include "utils/vector2i.hpp"
#include "SDL2/SDL_image.h"
#include "graph/Intersection.hpp" 
#include "graph/Road.hpp"
#include "simulation/VehicleManager.hpp"
#include "simulation/RouteOptimizer.hpp"
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
        const int reroutedVehicles =
            Game.vehicleManager.rerouteVehiclesAroundBannedRoads();
        Game.visualizationEngine.buildRenderCache(Game.graph);
        std::cout << "[INFO] Successfully loaded route ID: " << routeID 
                  << " (" << filePath << "), rerouted vehicles: "
                  << reroutedVehicles << "\n";
        return true;
    }

    std::cerr << "[ERROR] Failed to load route ID: " << routeID 
              << " from path: " << filePath << "\n";
    return false;
}

// Minimum pixel distance between press and release to count as a drag
// rather than a click.
static constexpr int DRAG_THRESHOLD = 5;

// State shared between handleInput() and wasDragAction().
static bool isDragging = false;
static int  lastMouseX = 0;
static int  lastMouseY = 0;
static int  dragStartX = 0;
static int  dragStartY = 0;
static bool mouseMoved_ = false;   // true when distance > DRAG_THRESHOLD

bool wasDragAction()
{
    return mouseMoved_;
}

void handleInput(SDL_Event& event, AppContext &Game)
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
            const float screenY = static_cast<float>(event.button.y);

            if (screenX >= 0)
            {
                Vector2 worldPos;
                worldPos.x = Game.camera.getX() + screenX / Game.camera.getZoom();
                worldPos.y = Game.camera.getY() + screenY / Game.camera.getZoom();
                //spawnVehicleAt(worldPos, Game.graph, Game.vehicleManager);

                isDragging = true;
                lastMouseX = event.button.x;
                lastMouseY = event.button.y;
                dragStartX = event.button.x;
                dragStartY = event.button.y;
                mouseMoved_ = false;
            }
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging = true;
            lastMouseX = event.button.x;
            lastMouseY = event.button.y;
            dragStartX = event.button.x;
            dragStartY = event.button.y;
            mouseMoved_ = false;
        }

        return;
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_LEFT ||
            event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging = false;
        }

        return;
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        if (isDragging)
        {
            int dx = event.motion.x - lastMouseX;
            int dy = event.motion.y - lastMouseY;

            Game.camera.offsetPosition(
                -static_cast<float>(dx) / Game.camera.getZoom(),
                -static_cast<float>(dy) / Game.camera.getZoom());

            lastMouseX = event.motion.x;
            lastMouseY = event.motion.y;

            // Check if the cumulative distance from the press origin
            // exceeds the threshold => treat as drag, not click.
            if (!mouseMoved_)
            {
                int totalDx = event.motion.x - dragStartX;
                int totalDy = event.motion.y - dragStartY;
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

void spawnVehicleAt(const Vector2& clickPos, 
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
