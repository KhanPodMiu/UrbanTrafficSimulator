#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "core/renderWindow.hpp"

#include "utils/vector2i.hpp"
#include "utils/event_handling.hpp"

#include "utils/MapLoader.hpp"
#include "visualization/MapRenderer.hpp"

#include "simulation/WorldClock.hpp"
#include "visualization/camera.hpp"

#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

#include <iostream>
#include <cmath>

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

constexpr int PANEL_WIDTH = 400;
constexpr int MAP_WIDTH = 1200;

constexpr int ROUNDABOUT_RADIUS = 80;
constexpr int ROAD_WIDTH = 80;

constexpr int INITIAL_CAMERA_SCALE = 900;

struct RoadRenderData
{
    Vector2 start;
    Vector2 end;

    float length;
    float angle;
};

Vector2 applyCamera(const Vector2& worldPos, const Camera& camera)
{
    return Vector2(worldPos.x - camera.getX(), worldPos.y - camera.getY());
}

//=======================================================================================================================================================================

int main(int argc, char* args[]) {

    //=======================================================================================================================================================================

    SDL_Init(SDL_INIT_EVERYTHING);
    RenderWindow* window = new RenderWindow("Urban Traffic", 1600, 900);

    //=======================================================================================================================================================================

    SDL_Texture* MapBackground = window -> loadTexture("assets/textures/Background.png");
    if(MapBackground == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    SDL_Texture* Intersection_Texture = window -> loadTexture("assets/textures/Intersection/temp_roundabout.png");
    if(Intersection_Texture == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    SDL_Texture* Road_Texture = window -> loadTexture("assets/textures/Roads/temp_road.png");
    if(Road_Texture == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    //=======================================================================================================================================================================

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

    std::vector<Vector2> intersections_location;
    std::vector<RoadRenderData> roads_location;
    
    for(const auto& [intersectionID, intersection] : graph.getIntersections()){
        Vector2 temp(intersection -> getX() - ROUNDABOUT_RADIUS, intersection -> getY() - ROUNDABOUT_RADIUS);
        intersections_location.push_back(temp);
    }


    for(const auto& [roadID, road] : graph.getRoads())
    {
        const Intersection* src = road->getSourceIntersection();
        const Intersection* dst = road->getDestinationIntersection();

        RoadRenderData temp;

        temp.start = Vector2(src->getX(), src->getY());

        temp.end = Vector2(dst->getX(), dst->getY());

        float dx = temp.end.x - temp.start.x;
        float dy = temp.end.y - temp.start.y;

        temp.length = sqrt(dx * dx + dy * dy);
        temp.angle = atan2(dy, dx) * 180.0 / M_PI;

        roads_location.push_back(temp);
    }

    //=======================================================================================================================================================================

    //Declaration area
    bool is_game_running = true;
    const double TARGET_FPS = 30.0;
    const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;


    //=======================================================================================================================================================================

    for(const auto& road : roads_location)
    {
        std::cout
            << "start=(" << road.start.x << "," << road.start.y << ") "
            << "end=(" << road.end.x << "," << road.end.y << ") "
            << "angle=" << road.angle << "\n";
    }
    
    while(is_game_running){

        //=======================================================================================================================================================================

        Uint64 frameStart = SDL_GetPerformanceCounter();
        clock.update();

        //=======================================================================================================================================================================
        while (SDL_PollEvent(&event)) {
            handleInput(event, is_game_running, camera);
        }

        //=======================================================================================================================================================================
        window -> clear();

        Vector2 BackGround_Position = applyCamera(Background_Point, camera);
        window->render(MapBackground, BackGround_Position);

        for(const auto& road : roads_location)
        {
            Vector2 renderPos = applyCamera(road.start, camera);

            window->renderRoad(Road_Texture, renderPos, road.length, ROAD_WIDTH, road.angle);
        }

        for(const auto& intersection : intersections_location)
        {
            Vector2 renderPos = applyCamera(intersection, camera);

            window->render(Intersection_Texture, renderPos, ROUNDABOUT_RADIUS);
        }

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