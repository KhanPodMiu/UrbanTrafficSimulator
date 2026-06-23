#include "TestGraphBuilder.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

#include <memory>
#include <vector>
#include <string>

Graph TestGraphBuilder::createGrid(
    int rows,
    int cols,
    int distance,
    int speedLimit,
    bool twoWay)
{
    Graph graph;

    // Store all intersections
    std::vector<
        std::vector<std::shared_ptr<Intersection>>
    > nodes(
        rows,
        std::vector<std::shared_ptr<Intersection>>(cols)
    );

    //--------------------------------------------------
    // Create intersections
    //--------------------------------------------------

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            std::string id =
                std::to_string(r) +
                "_" +
                std::to_string(c);

            auto intersection =
                std::make_shared<Intersection>(
                    id,
                    c * distance,
                    r * distance
                );

            graph.addIntersection(intersection);

            nodes[r][c] = intersection;
        }
    }

    //--------------------------------------------------
    // Create roads
    //--------------------------------------------------

    int roadID = 0;

    // Horizontal roads
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols - 1; c++)
        {
            auto from = nodes[r][c];
            auto to   = nodes[r][c + 1];

            auto road =
                std::make_shared<Road>(
                    "R" + std::to_string(roadID++),
                    from.get(),
                    to.get(),
                    distance,
                    speedLimit
                );

            graph.addRoad(road);

            if (twoWay)
            {
                auto reverseRoad =
                    std::make_shared<Road>(
                        "R" + std::to_string(roadID++),
                        to.get(),
                        from.get(),
                        distance,
                        speedLimit
                    );

                graph.addRoad(reverseRoad);
            }
        }
    }

    // Vertical roads
    for (int r = 0; r < rows - 1; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            auto from = nodes[r][c];
            auto to   = nodes[r + 1][c];

            auto road =
                std::make_shared<Road>(
                    "R" + std::to_string(roadID++),
                    from.get(),
                    to.get(),
                    distance,
                    speedLimit
                );

            graph.addRoad(road);

            if (twoWay)
            {
                auto reverseRoad =
                    std::make_shared<Road>(
                        "R" + std::to_string(roadID++),
                        to.get(),
                        from.get(),
                        distance,
                        speedLimit
                    );

                graph.addRoad(reverseRoad);
            }
        }
    }

    return graph;
}
