#pragma once

#include "vehicles/Vehicle.hpp"
#include <vector>
#include <memory>

class RenderWindow;
class Camera;
class TextureManager;

class VehicleRenderer
{
public:
    VehicleRenderer() = default;
    ~VehicleRenderer() = default;

    /// Draws the current dynamic vehicle list.
    void renderVehicles(
        RenderWindow& window,
        const Camera& camera,
        const std::vector<std::shared_ptr<Vehicle>>& vehicles,
        const TextureManager& textureManager);

    /// Draws the route lines and waypoints for a tracked vehicle.
    void renderVehicleRoute(
        RenderWindow& window,
        const Camera& camera,
        const Vehicle& vehicle) const;
};
