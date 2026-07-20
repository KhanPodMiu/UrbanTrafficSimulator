#pragma once

#include "SDL2/SDL.h"
#include "utils/vector2i.hpp"
#include <vector>
#include <memory>

class Graph;
class RenderWindow;
class Camera;
class Road;

class VisualizationEngine
{
public:
    VisualizationEngine();
    ~VisualizationEngine();

    bool loadAssets(RenderWindow& window);

    void buildRenderCache(const Graph& graph);

    void render(RenderWindow& window, const Camera& camera);

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

    SDL_Texture* greenLightTexture_ = nullptr;
    SDL_Texture* redLightTexture_ = nullptr;
    SDL_Texture* yellowLightTexture_ = nullptr;

    std::vector<Vector2> intersectionsLocation_;
    std::vector<RoadRenderData> roadsLocation_;
};
