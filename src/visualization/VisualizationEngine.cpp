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
    mapRenderer_.render(window, camera, textureManager_);

    if (heatMapOverlay_.isEnabled())
    {
        heatMapOverlay_.renderRoads(
            window,
            camera,
            mapRenderer_.getRoadsLocation(),
            textureManager_);
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
