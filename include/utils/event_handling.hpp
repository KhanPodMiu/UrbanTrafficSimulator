#pragma once

#include "visualization/camera.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "graph/Graph.hpp"
#include "simulation/VehicleManager.hpp"
#include "simulation/RouteOptimizer.hpp"
#include "utils/vector2i.hpp"

void handleInput(SDL_Event& event, bool& isRunning, Camera& camera, Graph& graph, VehicleManager& vehicleManager);
void spawnVehicleAt(const Vector2& clickPos, Graph& graph, VehicleManager& vehicleManager);