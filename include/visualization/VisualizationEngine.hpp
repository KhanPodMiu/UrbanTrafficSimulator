#pragma once

#include "SDL2/SDL.h"
#include "render/TextureManager.hpp"
#include "render/MapRenderer.hpp"
#include "render/VehicleRenderer.hpp"
#include "render/EntityPicker.hpp"
#include "render/HeatMapOverlay.hpp"
#include "vehicles/Vehicle.hpp"
#include <vector>
#include <memory>

class Graph;
class RenderWindow;
class Camera;
class Intersection;

class VisualizationEngine
{
public:
    VisualizationEngine();
    ~VisualizationEngine();

    /// Loads all texture assets. Delegates to TextureManager.
    bool loadAssets(RenderWindow& window);

    /// Pre-computes render data for roads and intersections. Delegates to MapRenderer.
    void buildRenderCache(const Graph& graph);

    /// Renders static map elements, traffic lights, and heat-map overlay if active.
    void render(RenderWindow& window, const Camera& camera);

    /// Handles heat-map events. Delegates to HeatMapOverlay.
    bool handleTrafficHeatMapEvent(const SDL_Event& event);

    /// Draws heat-map UI controls. Delegates to HeatMapOverlay.
    void renderTrafficHeatMapUi(RenderWindow& window) const;

    /// Returns true if traffic heat-map overlay is enabled.
    bool isTrafficHeatMapEnabled() const noexcept;

    /// Draws dynamic vehicle list. Delegates to VehicleRenderer.
    void renderVehicles(
        RenderWindow& window,
        const Camera& camera,
        const std::vector<std::shared_ptr<Vehicle>>& vehicles);

    /// Returns vehicle under mouse cursor. Delegates to EntityPicker.
    std::shared_ptr<Vehicle> pickVehicle(
        const Camera& camera,
        const std::vector<std::shared_ptr<Vehicle>>& vehicles,
        int mouseX,
        int mouseY) const;

    /// Returns intersection under mouse cursor. Delegates to EntityPicker.
    std::shared_ptr<Intersection> pickIntersection(
        const Camera& camera,
        int mouseX,
        int mouseY) const;

    /// Draws selection highlight around selected vehicle or intersection. Delegates to EntityPicker.
    void renderSelectionHighlight(
        RenderWindow& window,
        const Camera& camera,
        const std::shared_ptr<Vehicle>& selectedVehicle,
        const std::shared_ptr<Intersection>& selectedIntersection) const;

    /// Releases all loaded assets. Delegates to TextureManager.
    void cleanUp(RenderWindow& window);

    // Sub-component getters for direct access if needed
    TextureManager& getTextureManager() noexcept { return textureManager_; }
    const TextureManager& getTextureManager() const noexcept { return textureManager_; }

    MapRenderer& getMapRenderer() noexcept { return mapRenderer_; }
    const MapRenderer& getMapRenderer() const noexcept { return mapRenderer_; }

    VehicleRenderer& getVehicleRenderer() noexcept { return vehicleRenderer_; }
    const VehicleRenderer& getVehicleRenderer() const noexcept { return vehicleRenderer_; }

    EntityPicker& getEntityPicker() noexcept { return entityPicker_; }
    const EntityPicker& getEntityPicker() const noexcept { return entityPicker_; }

    HeatMapOverlay& getHeatMapOverlay() noexcept { return heatMapOverlay_; }
    const HeatMapOverlay& getHeatMapOverlay() const noexcept { return heatMapOverlay_; }

private:
    TextureManager textureManager_;
    MapRenderer mapRenderer_;
    VehicleRenderer vehicleRenderer_;
    EntityPicker entityPicker_;
    HeatMapOverlay heatMapOverlay_;
};
