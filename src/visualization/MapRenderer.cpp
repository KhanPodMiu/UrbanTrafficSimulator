#include "visualization/MapRenderer.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include "core/renderWindow.hpp"

void MapRenderer::render(RenderWindow& window, const Graph& graph) {
    constexpr float SCALE = 1.0;

    for (const auto& [roadID, road] : graph.getRoads()) {
        const Intersection* source = road->getSourceIntersection();
        const Intersection* destination = road->getDestinationIntersection();

        if (!source || !destination) continue;

        int x1 = static_cast<int>(source->getX() * SCALE);
        int y1 = static_cast<int>(source->getY() * SCALE);
        int x2 = static_cast<int>(destination->getX() * SCALE);
        int y2 = static_cast<int>(destination->getY() * SCALE);

        window.renderLine(x1, y1, x2, y2);
    }
}