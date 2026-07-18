// #define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "core/renderWindow.hpp"
#include "core/Constants.hpp"

#include "utils/vector2i.hpp"
#include "utils/event_handling.hpp"

#include "utils/MapLoader.hpp"
#include "visualization/VisualizationEngine.hpp"

#include "simulation/WorldClock.hpp"
#include "visualization/camera.hpp"

#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

#include <iostream>

//=======================================================================================================================================================================

int main(int argc, char* args[]) {

    //=======================================================================================================================================================================

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    RenderWindow window("Urban Traffic", Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);

    //=======================================================================================================================================================================

    VisualizationEngine visualizationEngine;
    if (!visualizationEngine.loadAssets(window)) {
        return 1;
    }

    //=======================================================================================================================================================================

    SDL_Event event;
    WorldClock clock;
    Camera camera;

    camera.setZoom(Config::INITIAL_CAMERA_SCALE);

    Graph graph;
    if(!MapLoader::loadFromJson("assets/maps/NguyenVanCu2.json", graph)){
        std::cerr << "Cannot load map\n";
        return 1;
    }

    visualizationEngine.buildRenderCache(graph);

    //=======================================================================================================================================================================

    bool is_game_running = true;

    //=======================================================================================================================================================================

    while(is_game_running){

        Uint64 frameStart = SDL_GetPerformanceCounter();
        clock.update();

        while (SDL_PollEvent(&event)) {
            handleInput(event, is_game_running, camera);
        }

        window.clear();

        visualizationEngine.render(window, camera);

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

    SDL_Quit();
    return 0;
}

