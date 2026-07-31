#include <gtest/gtest.h>

#include <memory>

#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

TEST(GraphTest, EnforcesSimpleDirectedMapContract)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    ASSERT_TRUE(graph.addIntersection(a));
    ASSERT_TRUE(graph.addIntersection(b));

    EXPECT_TRUE(graph.addRoad(
        std::make_shared<Road>("AB", a.get(), b.get(), 60)));
    EXPECT_FALSE(graph.addRoad(
        std::make_shared<Road>(
            "AB_DUPLICATE", a.get(), b.get(), 60)));
    EXPECT_TRUE(graph.addRoad(
        std::make_shared<Road>("BA", b.get(), a.get(), 60)));
    EXPECT_FALSE(graph.addRoad(
        std::make_shared<Road>(
            "A_LOOP", a.get(), a.get(), 60)));
    EXPECT_TRUE(graph.isValid());
}

TEST(GraphTest, RejectsEndpointObjectOwnedOutsideGraph)
{
    Graph graph;
    auto ownedA = std::make_shared<Intersection>("A", 0, 0);
    auto ownedB = std::make_shared<Intersection>("B", 1000, 0);
    ASSERT_TRUE(graph.addIntersection(ownedA));
    ASSERT_TRUE(graph.addIntersection(ownedB));

    auto externalA = std::make_shared<Intersection>("A", 0, 0);
    EXPECT_FALSE(graph.addRoad(
        std::make_shared<Road>(
            "EXTERNAL",
            externalA.get(),
            ownedB.get(),
            60)));
    EXPECT_TRUE(graph.getRoads().empty());
}

TEST(GraphTest, DetachesExternallyRetainedRoadOnRemoval)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    graph.addIntersection(a);
    graph.addIntersection(b);
    auto road =
        std::make_shared<Road>("AB", a.get(), b.get(), 60);
    ASSERT_TRUE(graph.addRoad(road));

    ASSERT_TRUE(graph.removeIntersection("A"));
    EXPECT_EQ(road->getSourceIntersection(), nullptr);
    EXPECT_EQ(road->getDestinationIntersection(), nullptr);
    EXPECT_EQ(graph.getRoad("AB"), nullptr);
}
