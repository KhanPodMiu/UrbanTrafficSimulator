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
    Config::PANEL_WIDTH + 18,
    18,
    154,
    42};

constexpr SDL_Rect HEAT_MAP_LEGEND_RECT{
    Config::PANEL_WIDTH + 18,
    68,
    218,
    124};
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
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    const SDL_Point mousePoint{mouseX, mouseY};
    const bool isHovered =
        SDL_PointInRect(&mousePoint, &HEAT_MAP_BUTTON_RECT) == SDL_TRUE;

    const SDL_Color buttonBackground = heatMapEnabled_
        ? SDL_Color{18, 65, 69, 238}
        : (isHovered
            ? SDL_Color{28, 58, 70, 238}
            : SDL_Color{18, 42, 53, 232});
    const SDL_Color buttonBorder = heatMapEnabled_
        ? SDL_Color{45, 212, 191, 255}
        : (isHovered
            ? SDL_Color{111, 147, 158, 255}
            : SDL_Color{70, 101, 111, 235});
    RenderHelpers::renderFloatingPanel(
        renderer,
        HEAT_MAP_BUTTON_RECT,
        buttonBackground,
        buttonBorder);

    const SDL_Color scaleColors[] = {
        TrafficHeatMapScale::colorFor(0),
        TrafficHeatMapScale::colorFor(50),
        TrafficHeatMapScale::colorFor(100)};
    for (int i = 0; i < 3; ++i)
    {
        SDL_SetRenderDrawColor(
            renderer,
            scaleColors[i].r,
            scaleColors[i].g,
            scaleColors[i].b,
            255);
        const SDL_Rect bar{
            HEAT_MAP_BUTTON_RECT.x + 13 + i * 5,
            HEAT_MAP_BUTTON_RECT.y + 13 - i * 2,
            3,
            15 + i * 2};
        SDL_RenderFillRect(renderer, &bar);
    }

    RenderHelpers::renderTextureAt(
        renderer,
        textureManager.getHeatMapButtonLabelTexture(),
        HEAT_MAP_BUTTON_RECT.x + 37,
        HEAT_MAP_BUTTON_RECT.y + 13);

    const SDL_Rect switchTrack{
        HEAT_MAP_BUTTON_RECT.x + HEAT_MAP_BUTTON_RECT.w - 33,
        HEAT_MAP_BUTTON_RECT.y + 14,
        22,
        14};
    SDL_SetRenderDrawColor(
        renderer,
        heatMapEnabled_ ? 19 : 72,
        heatMapEnabled_ ? 112 : 91,
        heatMapEnabled_ ? 102 : 98,
        255);
    SDL_RenderFillRect(renderer, &switchTrack);
    SDL_SetRenderDrawColor(renderer, 238, 246, 248, 255);
    RenderHelpers::fillCircle(
        renderer,
        heatMapEnabled_ ? switchTrack.x + 15 : switchTrack.x + 7,
        switchTrack.y + 7,
        5);

    if (!heatMapEnabled_)
        return;

    RenderHelpers::renderFloatingPanel(
        renderer,
        HEAT_MAP_LEGEND_RECT,
        SDL_Color{12, 34, 43, 226},
        SDL_Color{63, 91, 100, 225});
    RenderHelpers::renderTextureAt(
        renderer,
        textureManager.getHeatMapLegendTitleTexture(),
        HEAT_MAP_LEGEND_RECT.x + 14,
        HEAT_MAP_LEGEND_RECT.y + 11);

    SDL_SetRenderDrawColor(renderer, 70, 101, 111, 180);
    SDL_RenderDrawLine(
        renderer,
        HEAT_MAP_LEGEND_RECT.x + 14,
        HEAT_MAP_LEGEND_RECT.y + 35,
        HEAT_MAP_LEGEND_RECT.x + HEAT_MAP_LEGEND_RECT.w - 14,
        HEAT_MAP_LEGEND_RECT.y + 35);

    struct LegendRow
    {
        SDL_Color color;
        SDL_Texture* label;
    };
    const LegendRow rows[] = {
        {TrafficHeatMapScale::colorFor(0), textureManager.getHeatMapLowLabelTexture()},
        {TrafficHeatMapScale::colorFor(50), textureManager.getHeatMapModerateLabelTexture()},
        {TrafficHeatMapScale::colorFor(100), textureManager.getHeatMapHeavyLabelTexture()}};

    for (int i = 0; i < 3; ++i)
    {
        const int rowY = HEAT_MAP_LEGEND_RECT.y + 53 + i * 23;
        SDL_SetRenderDrawColor(
            renderer,
            rows[i].color.r,
            rows[i].color.g,
            rows[i].color.b,
            255);
        RenderHelpers::fillCircle(renderer, HEAT_MAP_LEGEND_RECT.x + 20, rowY + 6, 5);
        RenderHelpers::renderTextureAt(
            renderer,
            rows[i].label,
            HEAT_MAP_LEGEND_RECT.x + 34,
            rowY);
    }
}
