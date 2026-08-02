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

void EventManager::processEvents(AppContext& context, WorldClock& clock, StatisticsPanel& statisticsPanel, EntityInfoPanel& entityInfoPanel,VehicleManager& vehicleManager, Camera& camera, VisualizationEngine& visualizationEngine)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        const bool heatMapControlHandled =
            visualizationEngine.handleTrafficHeatMapEvent(event);

        if (!heatMapControlHandled)
            handleInput(event, context);

        const PanelCommand panelCommand =
            statisticsPanel.handleEvent(event);

        switch (panelCommand)
        {
            case PanelCommand::Start:
                clock.start();
                break;

            case PanelCommand::Pause:
                clock.pause();
                break;

            case PanelCommand::Restart:
                clock.reset();
                vehicleManager.reset();
                TrafficLightManager::getInstance().reset();
                entityInfoPanel.clearSelection();
                clock.start();
                break;

            case PanelCommand::SpeedChanged:
                clock.setSpeedMultiplier(
                    statisticsPanel.getSelectedSpeedMultiplier());
                break;

            case PanelCommand::AlgorithmChanged:
            case PanelCommand::None:
                break;
        }

        if (event.type == SDL_MOUSEBUTTONUP &&
            event.button.button == SDL_BUTTON_LEFT &&
            !heatMapControlHandled &&
            !wasDragAction() &&
            event.button.x >= Config::PANEL_WIDTH &&
            !entityInfoPanel.containsPoint(
                event.button.x,
                event.button.y))
        {
            const std::shared_ptr<Vehicle> selectedVehicle =
                visualizationEngine.pickVehicle(
                    camera,
                    vehicleManager.getVehicles(),
                    event.button.x,
                    event.button.y);

            if (selectedVehicle)
            {
                entityInfoPanel.selectVehicle(selectedVehicle);
            }
            else
            {
                const std::shared_ptr<Intersection>
                    selectedIntersection =
                        visualizationEngine.pickIntersection(
                            camera,
                            event.button.x,
                            event.button.y);

                if (selectedIntersection)
                {
                    entityInfoPanel.selectIntersection(
                        selectedIntersection);
                }
                else
                {
                    entityInfoPanel.clearSelection();
                }
            }
        }
    }
}