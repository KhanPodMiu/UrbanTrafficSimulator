#include "visualization/VisualizationEngine.hpp"

VisualizationEngine::VisualizationEngine() = default;
VisualizationEngine::~VisualizationEngine() = default;

bool VisualizationEngine::loadAssets(RenderWindow& window)
{
    return textureManager_.loadAssets(window);
}

void VisualizationEngine::buildRenderCache(const Graph& graph)
{
    mapRenderer_.buildRenderCache(graph);
}

void VisualizationEngine::render(RenderWindow& window, const Camera& camera)
{
    float zoom = camera.getZoom();

    Vector2 worldOrigin(0, 0);
    Vector2 bgPos = applyCamera(worldOrigin, camera);
    bgPos.x += Config::PANEL_WIDTH;
    //window.render(mapBackground_, bgPos, zoom);

    // Background
    SDL_Rect backgroundDestination = {
    static_cast<int>(std::lround(bgPos.x)),
    static_cast<int>(std::lround(bgPos.y)),
    static_cast<int>(std::lround(Config::MAP_WIDTH * zoom)),
    static_cast<int>(std::lround(Config::MAP_HEIGHT * zoom))
    };

    SDL_RenderCopy(
    window.getRenderer(),
    mapBackground_,
    nullptr,
    &backgroundDestination
    );

    for (const auto& road : roadsLocation_) {
        Vector2 shiftedStart(road.start.x + road.offsetX, road.start.y + road.offsetY);

        Vector2 renderPos = applyCamera(shiftedStart, camera);
        renderPos.x += Config::PANEL_WIDTH;

        SDL_Texture* currentRoadTex = (road.road && road.road->isVIPExclusive()) 
                                      ? bannedRoadTexture_ 
                                      : roadTexture_;

        const bool closurePending =
            road.road && road.road->isVIPClosurePending();
        if (closurePending)
        {
            // Amber marks the advance-warning/clearance phase. The road is
            // already excluded from routing, but cars currently inside may
            // still leave before the striped active closure appears.
            SDL_SetTextureColorMod(currentRoadTex, 255, 185, 70);
        }

        window.renderRoad(currentRoadTex, renderPos, road.length * zoom, Config::ROAD_WIDTH * zoom, road.angle);

        if (closurePending)
            SDL_SetTextureColorMod(currentRoadTex, 255, 255, 255);
    }

    if (heatMapEnabled_)
        renderTrafficHeatMapRoads(window, camera);

    // Intersection
    for (const auto& intersection : intersectionsLocation_) {
        Vector2 topLeft(
            intersection.center.x - Config::ROUNDABOUT_RADIUS,
            intersection.center.y - Config::ROUNDABOUT_RADIUS);
        Vector2 renderPos = applyCamera(topLeft, camera);
        renderPos.x += Config::PANEL_WIDTH;
        window.render(intersectionTexture_, renderPos, zoom, Config::ROUNDABOUT_RADIUS * zoom);
    }

    for (const auto& road : roadsLocation_) {
        if (road.road && road.road->isTrafficLightEnabled()) {
            Vector2 lightScreenPos = applyCamera(road.lightPos, camera);
            lightScreenPos.x += Config::PANEL_WIDTH;

            SDL_Texture* lightTex = trafficLightTexture(
                road.road->getTrafficLightState(),
                greenLightTexture_,
                yellowLightTexture_,
                redLightTexture_
            );

            int size = static_cast<int>(Config::TRAFFIC_LIGHT_MARKER_SIZE * zoom);
            window.renderTrafficLight(lightTex, (int)lightScreenPos.x, (int)lightScreenPos.y, size);
        }
    }

}

void VisualizationEngine::renderTrafficHeatMapRoads(
    RenderWindow& window,
    const Camera& camera) const
{
    if (heatMapOverlayTexture_ == nullptr)
        return;

    SDL_Renderer* renderer = window.getRenderer();
    const SDL_Rect viewport{
        Config::PANEL_WIDTH,
        0,
        Config::VIEW_PORT_WIDTH,
        Config::VIEW_PORT_HEIGHT};
    SDL_RenderSetClipRect(renderer, &viewport);

    if (heatMapOverlay_.isEnabled())
    {
        // Closed/VIP roads retain their dedicated striped texture so users do
        // not mistake an unavailable route for ordinary heavy traffic.
        // if (!road.road || road.road->isVIPExclusive()) // Legacy: active-only closure.
        if (!road.road || road.road->isUnavailableForRouting())
            continue;

        const SDL_Color color = TrafficHeatMapScale::colorFor(
            road.road->getCongestionLevel());
        SDL_SetTextureColorMod(
            heatMapOverlayTexture_, color.r, color.g, color.b);

        const Vector2 shiftedStart(
            road.start.x + road.offsetX,
            road.start.y + road.offsetY);
        Vector2 screenStart = applyCamera(shiftedStart, camera);
        screenStart.x += Config::PANEL_WIDTH;

        renderSolidRoadOverlay(
            renderer,
            heatMapOverlayTexture_,
            screenStart,
            road.length * zoom,
            overlayWidth,
            road.angle);
    }
}

bool VisualizationEngine::handleTrafficHeatMapEvent(const SDL_Event& event)
{
    return heatMapOverlay_.handleEvent(event);
}

void VisualizationEngine::renderTrafficHeatMapUi(RenderWindow& window) const
{
    heatMapOverlay_.renderUi(window, textureManager_);
}

bool VisualizationEngine::isTrafficHeatMapEnabled() const noexcept
{
    return heatMapOverlay_.isEnabled();
}

void VisualizationEngine::renderVehicles(
    RenderWindow& window,
    const Camera& camera,
    const std::vector<std::shared_ptr<Vehicle>>& vehicles)
{
    vehicleRenderer_.renderVehicles(window, camera, vehicles, textureManager_);
}

std::shared_ptr<Vehicle> VisualizationEngine::pickVehicle(
    const Camera& camera,
    const std::vector<std::shared_ptr<Vehicle>>& vehicles,
    int mouseX,
    int mouseY) const
{
    return entityPicker_.pickVehicle(camera, vehicles, mouseX, mouseY);
}

std::shared_ptr<Intersection> VisualizationEngine::pickIntersection(
    const Camera& camera,
    int mouseX,
    int mouseY) const
{
    return entityPicker_.pickIntersection(camera, mapRenderer_.getIntersectionsLocation(), mouseX, mouseY);
}

void VisualizationEngine::renderSelectionHighlight(
    RenderWindow& window,
    const Camera& camera,
    const std::shared_ptr<Vehicle>& selectedVehicle,
    const std::shared_ptr<Intersection>& selectedIntersection) const
{
    entityPicker_.renderSelectionHighlight(
        window,
        camera,
        selectedVehicle,
        selectedIntersection,
        vehicleRenderer_);
}

void VisualizationEngine::cleanUp(RenderWindow& window)
{
    textureManager_.cleanUp(window);
}
