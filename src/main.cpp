#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "core/renderWindow.hpp"
#include "simulation/WorldClock.hpp"
#include "utils/MapLoader.hpp"
#include "graph/Graph.hpp"
#include "visualization/MapRenderer.hpp"

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

constexpr int PANEL_WIDTH = 400;
constexpr int MAP_WIDTH = 1200;
constexpr int PANEL_TOP_LEFT_POINT = 1201;

int main(int argc, char* args[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    RenderWindow* window = new RenderWindow("Urban Traffic", 1600, 900);
    // SDL_Texture* MapBackground = window -> loadTexture("assets/maps/background.png");

    // if(MapBackground == nullptr){
    //     std::cerr << "Hey.. recheck the IMG PATH" << SDL_GetError();
    //     return 1;
    // }

    Graph graph;
    if(!MapLoader::loadFromJson("assets/maps/small_map.json", graph)){
        std::cerr << "Cannot load map\n";
        return 1;
    }
    MapRenderer mapRenderer;

    //Declaration area
    bool is_game_running = true;
    SDL_Event event;

    WorldClock clock;
    const double TARGET_FPS = 30.0;
    const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    while(is_game_running){
        Uint64 frameStart = SDL_GetPerformanceCounter();
        clock.update();

        // Poll events from the queue
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_game_running = false; // Handles the window's 'X' button
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_game_running = false; // Triggers loop exit on ESC
                }
            }
        }

        //Working area
        window -> clear();
        
        //UI Panel
        SDL_SetRenderDrawColor(window->getRenderer(), 40, 40, 40, 255);
        SDL_Rect panel = {PANEL_TOP_LEFT_POINT, 0, PANEL_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(window->getRenderer(), &panel);

        // window -> render(MapBackground);
        // mapRenderer.render(*window, graph);
        window -> display();

        


        //Timestepping
        Uint64 frameEnd = SDL_GetPerformanceCounter();
        double frameDuration = static_cast<double>(frameEnd - frameStart) / static_cast<double>(SDL_GetPerformanceFrequency());

        if (frameDuration < TARGET_FRAME_TIME) {
            Uint32 delayMs = static_cast<Uint32>((TARGET_FRAME_TIME - frameDuration) * 1000.0);
            SDL_Delay(delayMs);
        }
    }

    window -> cleanUp();
    // window -> cleanUpTexture(MapBackground);

    SDL_Quit();
    return 0;
}