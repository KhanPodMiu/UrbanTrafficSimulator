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

constexpr int ROUNDABOUT_RADIUS = 250;
constexpr int ROAD_WIDTH = 80;

constexpr float INITIAL_CAMERA_SCALE = 1.0f; // zoom so that a 5300px map fits in ~900px

struct RoadRenderData
{
    Vector2 start;
    Vector2 end;

    float length;
    float angle;

    float offsetX;
    float offsetY;
};

Vector2 applyCamera(const Vector2& worldPos, const Camera& camera)
{
    float zoom = camera.getZoom();
    float screenX = (worldPos.x - camera.getX()) * zoom;
    float screenY = (worldPos.y - camera.getY()) * zoom;
    return Vector2(screenX, screenY);
}

//=======================================================================================================================================================================

int main(int argc, char* args[]) {

    //=======================================================================================================================================================================

    // FIX (Bug 5): Check SDL_Init return value
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // FIX (Bug 4): Use stack object instead of raw new to avoid memory leak
    RenderWindow window("Urban Traffic", WINDOW_WIDTH, WINDOW_HEIGHT);

    //=======================================================================================================================================================================

    SDL_Texture* MapBackground = window.loadTexture("assets/textures/newtemp.png");
    if(MapBackground == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    SDL_Texture* Intersection_Texture = window.loadTexture("assets/textures/Intersection/rb1.png");
    if(Intersection_Texture == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    SDL_Texture* Road_Texture = window.loadTexture("assets/textures/Roads/road2.png");
    if(Road_Texture == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    //=======================================================================================================================================================================

    SDL_Event event;
    MapRenderer mapRenderer;
    WorldClock clock;
    Camera camera;

    camera.setZoom(INITIAL_CAMERA_SCALE);

    Graph graph;
    if(!MapLoader::loadFromJson("assets/maps/small_map.json", graph)){
        std::cerr << "Cannot load map\n";
        return 1;
    }

    std::vector<Vector2> intersections_location;
    std::vector<RoadRenderData> roads_location;

    for(const auto& [intersectionID, intersection] : graph.getIntersections()){
        Vector2 temp(intersection->getX() - ROUNDABOUT_RADIUS, intersection->getY() - ROUNDABOUT_RADIUS);
        intersections_location.push_back(temp);
    }

    for(const auto& [roadID, road] : graph.getRoads())
    {
        const Intersection* src = road->getSourceIntersection();
        const Intersection* dst = road->getDestinationIntersection();

        RoadRenderData temp;

        temp.start = Vector2(src->getX(), src->getY());
        temp.end   = Vector2(dst->getX(), dst->getY());

        float dx = temp.end.x - temp.start.x;
        float dy = temp.end.y - temp.start.y;

        temp.length = std::sqrt(dx * dx + dy * dy);
        temp.angle  = std::atan2(dy, dx) * 180.0f / static_cast<float>(M_PI);

        float angle_rad = std::atan2(dy, dx);
        float halfWidth = ROAD_WIDTH * 0.5f;
        temp.offsetX = -std::sin(angle_rad) * halfWidth;
        temp.offsetY =  std::cos(angle_rad) * halfWidth;

        roads_location.push_back(temp);
    }

    //=======================================================================================================================================================================

    bool is_game_running = true;
    const double TARGET_FPS = 30.0;
    const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    //=======================================================================================================================================================================

    while(is_game_running){

        Uint64 frameStart = SDL_GetPerformanceCounter();
        clock.update();

        while (SDL_PollEvent(&event)) {
            handleInput(event, is_game_running, camera);
        }

        window.clear();

        float zoom = camera.getZoom();

        Vector2 worldOrigin(0, 0);
        Vector2 bgPos = applyCamera(worldOrigin, camera);
        bgPos.x += PANEL_WIDTH;
        window.render(MapBackground, bgPos, zoom);

        for(const auto& road : roads_location)
        {
            Vector2 shiftedStart(road.start.x + road.offsetX,
                                 road.start.y + road.offsetY);

            Vector2 renderPos = applyCamera(shiftedStart, camera);
            renderPos.x += PANEL_WIDTH;

            window.renderRoad(Road_Texture, renderPos, road.length * zoom, ROAD_WIDTH * zoom, road.angle);
        }

        // Intersection
        for(const auto& intersection : intersections_location)
        {
            Vector2 renderPos = applyCamera(intersection, camera);
            renderPos.x += PANEL_WIDTH;
            window.render(Intersection_Texture, renderPos, zoom, ROUNDABOUT_RADIUS * zoom);
        }

        // UI Panel
        SDL_SetRenderDrawColor(window.getRenderer(), 40, 40, 40, 255);
        SDL_Rect panel = {0, 0, PANEL_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(window.getRenderer(), &panel);

        window.display();

        Uint64 frameEnd = SDL_GetPerformanceCounter();
        double frameDuration = static_cast<double>(frameEnd - frameStart) /
                               static_cast<double>(SDL_GetPerformanceFrequency());

        if (frameDuration < TARGET_FRAME_TIME) {
            Uint32 delayMs = static_cast<Uint32>((TARGET_FRAME_TIME - frameDuration) * 1000.0);
            SDL_Delay(delayMs);
        }
    }

    //=======================================================================================================================================================================

    // FIX (Bug 3): clean up ALL textures, not just MapBackground
    window.cleanUpTexture(MapBackground);
    window.cleanUpTexture(Intersection_Texture);
    window.cleanUpTexture(Road_Texture);

    window.cleanUp();

    SDL_Quit();
    return 0;
}