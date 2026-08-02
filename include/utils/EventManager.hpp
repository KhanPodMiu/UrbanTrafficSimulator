#pragma once

#include "utils/EventManager.hpp"
#include "utils/event_handling.hpp"

#include "algorithms/RoutingManager.hpp"
#include "algorithms/Dijkstra.hpp"
        
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "core/renderWindow.hpp"
#include "core/Constants.hpp"

#include "utils/vector2i.hpp"
#include "utils/event_handling.hpp"

#include "utils/MapLoader.hpp"
#include "utils/EventManager.hpp"
#include "visualization/VisualizationEngine.hpp"

#include "simulation/WorldClock.hpp"
#include "simulation/TrafficLightManager.hpp"
#include "simulation/RouteOptimizer.hpp"
#include "simulation/VehicleManager.hpp"
#include "visualization/camera.hpp"
#include "render/EntityInfoPanel.hpp"
#include "render/StatisticsPanel.hpp"

#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

//I really don't know which files were needed so I added them all =))))

struct AppContext;

class EventManager {
public:
    static void processEvents(AppContext& context, WorldClock& clock, StatisticsPanel& statisticsPanel, EntityInfoPanel& entityInfoPanel, VehicleManager& vehicleManager, Camera& camera, VisualizationEngine& visualizationEngine);
};