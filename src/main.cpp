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

#include <iostream>

//=======================================================================================================================================================================

int main(int argc, char* argv[]) {

    //=======================================================================================================================================================================

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError()<< std::endl;
        SDL_Quit();
        return 1;
    }

    RenderWindow window("Urban Traffic", Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);

    //=======================================================================================================================================================================

    VisualizationEngine visualizationEngine;
    if (!visualizationEngine.loadAssets(window)) {
        return 1;
    }
    //======================================================================================================================================================================

    StatisticsPanel statisticsPanel;

    if (!statisticsPanel.loadAssets(window))
    {
        std::cerr << "Cannot load StatisticsPanel assets"
                << std::endl;

        visualizationEngine.cleanUp(window);
        window.cleanUp();
        SDL_Quit();

        return 1;
    }

    EntityInfoPanel entityInfoPanel;
    if (!entityInfoPanel.loadAssets())
    {
        std::cerr << "Cannot load EntityInfoPanel assets" << std::endl;
        statisticsPanel.cleanUp(window);
        visualizationEngine.cleanUp(window);
        window.cleanUp();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    //=======================================================================================================================================================================

    SDL_Event event;
    WorldClock clock;
    Camera camera;

    camera.setZoom(Config::INITIAL_CAMERA_SCALE);

    Graph graph;
    if(!MapLoader::loadFromJson("assets/maps/khapkhap.json", graph)){
        std::cerr << "Cannot load map\n";
        return 1;
    }

    visualizationEngine.buildRenderCache(graph);

    TrafficLightManager::getInstance().initializeTopology(graph);


    RoutingManager routingManager;

    routingManager.setStrategy(std::make_unique<Dijkstra>());
    // routingManager.setStrategy(std::make_unique<BFS>());
    // routingManager.setStrategy(std::make_unique<AStarStrategy>());

    // // Tìm đường từ I1 -> I3
    // RouteRequest request("I1", "I3");

    // RouteResult result = routingManager.calculateRoute(graph, request);

    // if (result.isSuccess)
    // {
    //     std::cout << "========== ROUTE ==========\n";
    //     std::cout << "Total Cost: "
    //             << result.totalCost
    //             << "\n\n";

    //     std::cout << "Path:\n";

    //     for (const auto& intersectionID : result.intersectionIDs)
    //     {
    //         std::cout << intersectionID << " ";
    //     }

    //     std::cout << "\n===========================\n";
    // }
    // else
    // {
    //     std::cout << "Cannot find route!\n";
    // }


    auto routeOptimizer = std::make_shared<RouteOptimizer>(&routingManager);

    VehicleManager vehicleManager(graph, routingManager, routeOptimizer, /*maxVehicles=*/300, /*spawnIntervalSeconds=*/0.5);

    //=======================================================================================================================================================================

    bool is_game_running = true;

    //=======================================================================================================================================================================

    while(is_game_running){

        Uint64 frameStart = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&event)) {
            handleInput(event, is_game_running, camera);

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

        clock.update();

        if (clock.isRunning())
        {
            TrafficLightManager::getInstance().update(clock.getDeltaTime());
            vehicleManager.update(clock.getDeltaTime());
        }

        window.clear();

        visualizationEngine.render(window, camera);
        visualizationEngine.renderVehicles(window, camera, vehicleManager.getVehicles());
        visualizationEngine.renderSelectionHighlight(
            window,
            camera,
            entityInfoPanel.getSelectedVehicle(),
            entityInfoPanel.getSelectedIntersection());
        entityInfoPanel.render(window);

        // SDL_SetRenderDrawColor(window.getRenderer(), 40, 40, 40, 255);
        // SDL_Rect panel = {0, 0, Config::PANEL_WIDTH, Config::WINDOW_HEIGHT};
        // SDL_RenderFillRect(window.getRenderer(), &panel);

        statisticsPanel.render(window,clock.getSimulationTime());

        window.display();

        Uint64 frameEnd = SDL_GetPerformanceCounter();
        double frameDuration = static_cast<double>(frameEnd - frameStart) /
                         static_cast<double>(SDL_GetPerformanceFrequency());

        if (frameDuration < Config::TARGET_FRAME_TIME) {
            Uint32 delayMs = static_cast<Uint32>((Config::TARGET_FRAME_TIME - frameDuration) * 1000.0);
            SDL_Delay(delayMs);
        }
    }

    //=======================================================================================================================================================================
    entityInfoPanel.cleanUp();
    statisticsPanel.cleanUp(window);
    visualizationEngine.cleanUp(window);

    window.cleanUp();

    TTF_Quit();
    SDL_Quit();
    return 0;
}
