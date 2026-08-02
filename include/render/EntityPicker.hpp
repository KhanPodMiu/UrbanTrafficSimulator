#pragma once

#include "render/MapRenderer.hpp"
#include "vehicles/Vehicle.hpp"
#include "graph/Intersection.hpp"
#include <vector>
#include <memory>

class RenderWindow;
class Camera;
class VehicleRenderer;

class EntityPicker
{
public:
    EntityPicker() = default;
    ~EntityPicker() = default;

    std::shared_ptr<Vehicle> pickVehicle(
        const Camera& camera,
        const std::vector<std::shared_ptr<Vehicle>>& vehicles,
        int mouseX,
        int mouseY) const;

    std::shared_ptr<Intersection> pickIntersection(
        const Camera& camera,
        const std::vector<MapRenderer::IntersectionRenderData>& intersections,
        int mouseX,
        int mouseY) const;

    void renderSelectionHighlight(
        RenderWindow& window,
        const Camera& camera,
        const std::shared_ptr<Vehicle>& selectedVehicle,
        const std::shared_ptr<Intersection>& selectedIntersection,
        const VehicleRenderer& vehicleRenderer) const;
};
