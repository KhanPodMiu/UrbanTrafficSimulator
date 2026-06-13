#include <gtest/gtest.h>
#include "utils/MapLoader.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

TEST(MapLoaderTest, LoadValidMap)
{
    Graph graph;

    bool result =
        MapLoader::loadFromJson(
            "assets/maps/small_map.json",
            graph
        );

    EXPECT_TRUE(result);

    EXPECT_NE(
        graph.getIntersection("I1"),
        nullptr
    );

    EXPECT_NE(
        graph.getIntersection("I2"),
        nullptr
    );

    EXPECT_NE(
        graph.getRoad("R1"),
        nullptr
    );

    EXPECT_NE(
        graph.getRoad("R2"),
        nullptr
    );

    EXPECT_TRUE(graph.isValid());
}

TEST(MapLoaderTest, MissingFile)
{
    Graph graph;

    EXPECT_FALSE(
        MapLoader::loadFromJson(
            "assets/maps/not_exist.json",
            graph
        )
    );
}

TEST(MapLoaderTest, DuplicateIntersectionID)
{
    Graph graph;

    EXPECT_FALSE(
        MapLoader::loadFromJson(
            "assets/maps/duplicate_intersection.json",
            graph
        )
    );
}