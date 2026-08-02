#include "render/HeatMapOverlay.hpp"
#include "render/TextureManager.hpp"
#include "render/RenderHelpers.hpp"
#include "render/TrafficHeatMap.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Road.hpp"
#include <algorithm>

namespace {
constexpr Uint8 HEAT_MAP_ROAD_ALPHA = 178;

constexpr SDL_Rect HEAT_MAP_BUTTON_RECT{
    324,
    621,
    85,
    27};
}

bool HeatMapOverlay::handleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN &&
        event.key.repeat == 0 &&
        event.key.keysym.sym == SDLK_h)
    {
        heatMapEnabled_ = !heatMapEnabled_;
        return true;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        const SDL_Point point{event.button.x, event.button.y};
        if (SDL_PointInRect(&point, &HEAT_MAP_BUTTON_RECT) == SDL_TRUE)
        {
            heatMapButtonPressed_ = true;
            return true;
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        heatMapButtonPressed_)
    {
        const SDL_Point point{event.button.x, event.button.y};
        if (SDL_PointInRect(&point, &HEAT_MAP_BUTTON_RECT) == SDL_TRUE)
            heatMapEnabled_ = !heatMapEnabled_;

        heatMapButtonPressed_ = false;
        return true;
    }

    return false;
}

void HeatMapOverlay::renderRoads(
    RenderWindow& window,
    const Camera& camera,
    const std::vector<MapRenderer::RoadRenderData>& roads,
    const TextureManager& textureManager) const
{
    SDL_Texture* overlayTexture = textureManager.getHeatMapOverlayTexture();
    if (overlayTexture == nullptr)
        return;

    SDL_Renderer* renderer = window.getRenderer();
    const SDL_Rect viewport{
        Config::PANEL_WIDTH,
        0,
        Config::VIEW_PORT_WIDTH,
        Config::VIEW_PORT_HEIGHT};
    SDL_RenderSetClipRect(renderer, &viewport);

    const float zoom = camera.getZoom();
    const float overlayWidth = std::max(
        3.0f,
        Config::ROAD_WIDTH * zoom * 0.72f);
    SDL_SetTextureAlphaMod(overlayTexture, HEAT_MAP_ROAD_ALPHA);

    for (const auto& road : roads)
    {
        if (!road.road || road.road->isUnavailableForRouting())
            continue;

        const SDL_Color color = TrafficHeatMapScale::colorFor(
            road.road->getCongestionLevel());
        SDL_SetTextureColorMod(
            overlayTexture, color.r, color.g, color.b);

        const Vector2 shiftedStart(
            road.start.x + road.offsetX,
            road.start.y + road.offsetY);
        Vector2 screenStart = RenderHelpers::applyCamera(shiftedStart, camera);
        screenStart.x += Config::PANEL_WIDTH;

        RenderHelpers::renderSolidRoadOverlay(
            renderer,
            overlayTexture,
            screenStart,
            road.length * zoom,
            overlayWidth,
            road.angle);
    }

    SDL_SetTextureColorMod(overlayTexture, 255, 255, 255);
    SDL_SetTextureAlphaMod(overlayTexture, 255);
    SDL_RenderSetClipRect(renderer, nullptr);
}

void HeatMapOverlay::renderUi(
    RenderWindow& window,
    const TextureManager& textureManager) const
{
    SDL_Renderer* renderer = window.getRenderer();
    SDL_Texture* buttonTexture = heatMapEnabled_
        ? textureManager.getHeatMapOnButtonTexture()
        : textureManager.getHeatMapOffButtonTexture();

    if (buttonTexture != nullptr)
    {
        SDL_RenderCopy(
            renderer,
            buttonTexture,
            nullptr,
            &HEAT_MAP_BUTTON_RECT);
    }
}
