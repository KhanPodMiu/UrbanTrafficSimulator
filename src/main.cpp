#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "core/renderWindow.hpp"
#include "simulation/WorldClock.hpp"
#include "utils/MapLoader.hpp"
#include "graph/Graph.hpp"
#include "visualization/MapRenderer.hpp"
#include "utils/vector2i.hpp"
#include "visualization/camera.hpp"

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

constexpr int PANEL_WIDTH = 400;
constexpr int MAP_WIDTH = 1200;

constexpr int ROUNDABOUT_RADIUS = 80;

constexpr int INITIAL_CAMERA_SCALE = 900;

//=======================================================================================================================================================================

int main(int argc, char* args[]) {

    //=======================================================================================================================================================================

    SDL_Init(SDL_INIT_EVERYTHING);
    RenderWindow* window = new RenderWindow("Urban Traffic", 1600, 900);

    SDL_Texture* MapBackground = window -> loadTexture("assets/textures/Background.png");
    if(MapBackground == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    Vector2 Background_Point(PANEL_WIDTH, 0);

    //=======================================================================================================================================================================

    SDL_Event event;
    MapRenderer mapRenderer;
    WorldClock clock;
    Camera camera;

    Graph graph;
    if(!MapLoader::loadFromJson("assets/maps/small_map.json", graph)){
        std::cerr << "Cannot load map\n";
        return 1;
    }

    //Declaration area
    bool is_game_running = true;
    const double TARGET_FPS = 30.0;
    const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;


    //=======================================================================================================================================================================
    
    while(is_game_running){

        //=======================================================================================================================================================================

        Uint64 frameStart = SDL_GetPerformanceCounter();
        clock.update();

        //=======================================================================================================================================================================

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_game_running = false;
            } 
            else if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        is_game_running = false;
                        break;
                    case SDLK_w:
                        camera.subY();
                        break;
                    case SDLK_a:
                        camera.subX();
                        break;
                    case SDLK_s:
                        camera.addY();
                        break;
                    case SDLK_d:
                        camera.addX();
                        break;
                }
            }
        }
        //=======================================================================================================================================================================
        window -> clear();
        

        //Background
        Background_Point.x -= camera.getX();
        Background_Point.y -= camera.getY();
        window -> render(MapBackground, Background_Point);
        Background_Point.x += camera.getX();
        Background_Point.y += camera.getY();


        //UI Panel
        SDL_SetRenderDrawColor(window->getRenderer(), 40, 40, 40, 255);
        SDL_Rect panel = {0, 0, PANEL_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(window->getRenderer(), &panel);

        window -> display();
        //=======================================================================================================================================================================

        Uint64 frameEnd = SDL_GetPerformanceCounter();
        double frameDuration = static_cast<double>(frameEnd - frameStart) / static_cast<double>(SDL_GetPerformanceFrequency());

        if (frameDuration < TARGET_FRAME_TIME) {
            Uint32 delayMs = static_cast<Uint32>((TARGET_FRAME_TIME - frameDuration) * 1000.0);
            SDL_Delay(delayMs);
        }

        //=======================================================================================================================================================================
    }

    //=======================================================================================================================================================================

    window -> cleanUp();
    window -> cleanUpTexture(MapBackground);

    SDL_Quit();
    return 0;
}