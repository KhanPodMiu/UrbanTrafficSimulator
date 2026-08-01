#pragma once

#include "visualization/camera.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "graph/Graph.hpp"
#include "simulation/VehicleManager.hpp"
#include "simulation/RouteOptimizer.hpp"
#include "utils/vector2i.hpp"
#include <filesystem>

class VisualizationEngine;
class PresidentialRouteManager;

struct AppContext {
    bool& isRunning;
    Camera& camera;
    Graph& graph;
    VehicleManager& vehicleManager;
    VisualizationEngine& visualizationEngine;
    bool& isBannedState;
    PresidentialRouteManager& presidentialRouteManager;
};

std::filesystem::path resolveAssetPath(const std::filesystem::path& relativePath);
void handleInput(SDL_Event& event, AppContext &Game);
void spawnVehicleAt(const Vector2& clickPos, Graph& graph, VehicleManager& vehicleManager);

// Returns true if the most recently completed mouse gesture was a drag
// (mouse moved more than a small threshold between press and release).
bool wasDragAction();

//helper function
bool switchBannedRoute(int routeID, AppContext &Game);
