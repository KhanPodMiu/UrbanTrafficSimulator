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

constexpr int ROUNDABOUT_RADIUS = 100;
constexpr int ROAD_WIDTH = 80;

// FIX (Bug 6): INITIAL_CAMERA_SCALE is now actually passed to the camera below
constexpr float INITIAL_CAMERA_SCALE = 0.17f; // zoom so that a 5300px map fits in ~900px

struct RoadRenderData
{
    Vector2 start;
    Vector2 end;

    float length;
    float angle;

    // Perpendicular offset (world-space) so one-way roads don't overlap.
    // Computed once at load time: (-sin(angle_rad), cos(angle_rad)) * ROAD_WIDTH/2
    float offsetX;
    float offsetY;
};

Vector2 applyCamera(const Vector2& worldPos, const Camera& camera)
{
    float zoom = camera.getZoom();
    return Vector2((worldPos.x - camera.getX()) * zoom, (worldPos.y - camera.getY()) * zoom);
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

    SDL_Texture* Intersection_Texture = window.loadTexture("assets/textures/Intersection/temp_roundabout.png");
    if(Intersection_Texture == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    SDL_Texture* Road_Texture = window.loadTexture("assets/textures/Roads/temp_road.png");
    if(Road_Texture == nullptr){
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    //=======================================================================================================================================================================

    SDL_Event event;
    MapRenderer mapRenderer;
    WorldClock clock;
    Camera camera;

    // FIX (Bug 6): Actually use the constant to initialise camera zoom
    camera.setZoom(INITIAL_CAMERA_SCALE);

    Graph graph;
    if(!MapLoader::loadFromJson("assets/maps/complex_map.json", graph)){
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

        // FIX (Bug 2): Compute perpendicular offset for one-way road lane separation.
        // The perpendicular to direction (cos θ, sin θ) is (-sin θ, cos θ).
        // We shift each road half a road-width to the RIGHT of its travel direction,
        // so that two opposing roads sit side by side without overlapping.
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

        // Road — FIX (Bug 2): apply perpendicular offset so one-way roads don't overlap
        for(const auto& road : roads_location)
        {
            // Shift the start point by the (already world-space) offset, then camera-transform
            Vector2 shiftedStart(road.start.x + road.offsetX,
                                 road.start.y + road.offsetY);

            Vector2 renderPos = applyCamera(shiftedStart, camera);
            renderPos.x += PANEL_WIDTH;

            // FIX (Bug 1): pass floats — renderRoad signature updated to accept float
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
    // FIX (Bug 4): no delete needed — window is now a stack object

    SDL_Quit();
    return 0;
}