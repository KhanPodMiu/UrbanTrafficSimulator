#pragma once

#include "SDL2/SDL.h"
#include "utils/vector2i.hpp"
#include <vector>
#include <memory>

class Graph;
class RenderWindow;
class Camera;
class Road;

// Owns everything needed to draw the map each frame: the textures, the
// precomputed per-road/per-intersection render data (so expensive math like
// sqrt/atan2 only runs once when the map loads, not every frame), and the
// draw calls themselves (background, roads, intersections, UI panel).
//
// This is a direct extraction of what used to live inline in main.cpp
// (RoadRenderData struct, applyCamera(), the cache-building loops, and the
// per-frame draw loop) — no math or ordering was changed.
class VisualizationEngine
{
public:
    VisualizationEngine();
    ~VisualizationEngine();

    // Loads the background/intersection/road textures. Returns false (and
    // prints an SDL error) if any texture fails to load.
    bool loadAssets(RenderWindow& window);

    // Precomputes intersection screen-space anchor points and per-road
    // length/angle/lane-offset data from the graph. Call once after the map
    // is loaded (or whenever the map changes).
    void buildRenderCache(const Graph& graph);

    // Draws background, roads, intersections and the left UI panel for the
    // current camera position/zoom. Does NOT call window.clear()/display().
    void render(RenderWindow& window, const Camera& camera);

    // Destroys the loaded textures.
    void cleanUp(RenderWindow& window);

private:
    struct RoadRenderData
    {
        Vector2 start;
        Vector2 end;
        float length;
        float angle;
        float offsetX;
        float offsetY;

        std::shared_ptr<Road> road;

        Vector2 lightPos;
    };

    Vector2 applyCamera(const Vector2& worldPos, const Camera& camera) const;

    SDL_Texture* mapBackground_ = nullptr;
    SDL_Texture* intersectionTexture_ = nullptr;
    SDL_Texture* roadTexture_ = nullptr;

    std::vector<Vector2> intersectionsLocation_;
    std::vector<RoadRenderData> roadsLocation_;
};
