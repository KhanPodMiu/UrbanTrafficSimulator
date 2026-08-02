#include "algorithms/RoutingManager.hpp"
#include "algorithms/AStarStrategy.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/Dijkstra.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
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

#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

#include <iostream>
#include <filesystem>

static std::filesystem::path getExecutableDir() {
    char* basePath = SDL_GetBasePath();
    if (basePath) {
        std::filesystem::path path(basePath);
        SDL_free(basePath);
        return path;
    }
    return std::filesystem::current_path();
}

static std::filesystem::path resolveAssetPath(const std::filesystem::path& relativePath) {
    namespace fs = std::filesystem;

    fs::path cwd = fs::current_path();
    fs::path exeDir = getExecutableDir();

    fs::path candidate = cwd / relativePath;
    if (fs::exists(candidate)) {
        return fs::canonical(candidate);
    }

    candidate = exeDir / relativePath;
    if (fs::exists(candidate)) {
        return fs::canonical(candidate);
    }

    candidate = exeDir.parent_path() / relativePath;
    if (fs::exists(candidate)) {
        return fs::canonical(candidate);
    }

    return relativePath;
}

//=======================================================================================================================================================================

int main(int argc, char* argv[]) {

    //=======================================================================================================================================================================

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    RenderWindow window("Urban Traffic", Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);

    //=======================================================================================================================================================================

    VisualizationEngine visualizationEngine;
    const std::filesystem::path assetsRoot = resolveAssetPath("assets");
    if (!std::filesystem::exists(assetsRoot)) {
        std::cerr << "Assets directory not found: " << assetsRoot << "\n";
        visualizationEngine.cleanUp(window);
        window.cleanUp();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (!visualizationEngine.loadAssets(window, assetsRoot)) {
        IMG_Quit();
        return 1;
    }

    //=======================================================================================================================================================================

    SDL_Event event;
    WorldClock clock;
    Camera camera;

    camera.setZoom(Config::INITIAL_CAMERA_SCALE);

    Graph graph;
    const std::filesystem::path mapFilePath = resolveAssetPath("assets/maps/khapkhap.json");
    if(!MapLoader::loadFromJson(mapFilePath.string(), graph)){
        std::cerr << "Cannot load map: " << mapFilePath << "\n";
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
        clock.update();

        TrafficLightManager::getInstance().update(clock.getDeltaTime());

        vehicleManager.update(clock.getDeltaTime());

        while (SDL_PollEvent(&event)) {
            handleInput(event, is_game_running, camera, graph, vehicleManager);
        }

        window.clear();

        visualizationEngine.render(window, camera);
        visualizationEngine.renderVehicles(window, camera, vehicleManager.getVehicles());

        SDL_SetRenderDrawColor(window.getRenderer(), 40, 40, 40, 255);
        SDL_Rect panel = {0, 0, Config::PANEL_WIDTH, Config::WINDOW_HEIGHT};
        SDL_RenderFillRect(window.getRenderer(), &panel);

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

    visualizationEngine.cleanUp(window);

    window.cleanUp();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
