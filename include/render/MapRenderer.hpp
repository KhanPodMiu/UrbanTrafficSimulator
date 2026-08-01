#pragma once

#include "utils/vector2i.hpp"
#include <vector>
#include <memory>

class Graph;
class Road;
class Intersection;
class RenderWindow;
class Camera;
class TextureManager;

class MapRenderer
{
public:
    struct IntersectionRenderData
    {
        Vector2 center;
        std::shared_ptr<Intersection> intersection;
    };

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

    MapRenderer() = default;
    ~MapRenderer() = default;

    /// Pre-computes render data for roads and intersections (called once after map load).
    void buildRenderCache(const Graph& graph);

    /// Renders the static map elements: background, roads, intersections, and traffic lights.
    void render(
        RenderWindow& window,
        const Camera& camera,
        const TextureManager& textureManager);

    const std::vector<IntersectionRenderData>& getIntersectionsLocation() const noexcept
    {
        return intersectionsLocation_;
    }

    const std::vector<RoadRenderData>& getRoadsLocation() const noexcept
    {
        return roadsLocation_;
    }

private:
    std::vector<IntersectionRenderData> intersectionsLocation_;
    std::vector<RoadRenderData> roadsLocation_;
};
