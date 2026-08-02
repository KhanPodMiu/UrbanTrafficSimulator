#pragma once

#include "render/MapRenderer.hpp"
#include "SDL2/SDL.h"
#include <vector>

class RenderWindow;
class Camera;
class TextureManager;

class HeatMapOverlay
{
public:
    HeatMapOverlay() = default;
    ~HeatMapOverlay() = default;

    bool handleEvent(const SDL_Event& event);

    void renderRoads(
        RenderWindow& window,
        const Camera& camera,
        const std::vector<MapRenderer::RoadRenderData>& roads,
        const TextureManager& textureManager) const;

    void renderUi(
        RenderWindow& window,
        const TextureManager& textureManager) const;

    bool isEnabled() const noexcept { return heatMapEnabled_; }

private:
    bool heatMapEnabled_ = false;
    bool heatMapButtonPressed_ = false;
};
