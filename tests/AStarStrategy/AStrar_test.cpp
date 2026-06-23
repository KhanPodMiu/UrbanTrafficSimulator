#include <gtest/gtest.h>

#include "algorithms/AStarStrategy.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "TestGraphBuilder.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

TEST(TestGraphBuilder, Create3x3)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    EXPECT_NE(
        graph.getIntersection("0_0"),
        nullptr);

    EXPECT_NE(
        graph.getIntersection("2_2"),
        nullptr);

    EXPECT_EQ(
        graph.getConnectedRoads("1_1").size(),
        4);
}

TEST(AStarStrategy, CanFindRoute)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    RouteRequest request(
        "0_0",
        "2_2");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_TRUE(result.isValid);
}

TEST(AStarStrategy, PathIsNotEmpty)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    RouteRequest request(
        "0_0",
        "2_2");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_FALSE(
        result.intersectionIDs.empty());
}

TEST(AStarStrategy, CorrectStartNode)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    RouteRequest request(
        "0_0",
        "2_2");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_EQ(
        result.intersectionIDs.front(),
        "0_0");
}

TEST(AStarStrategy, CorrectDestinationNode)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    RouteRequest request(
        "0_0",
        "2_2");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_EQ(
        result.intersectionIDs.back(),
        "2_2");
}

TEST(AStarStrategy, SameStartAndDestination)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    RouteRequest request(
        "1_1",
        "1_1");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_TRUE(result.isValid);

    EXPECT_EQ(
        result.intersectionIDs.size(),
        1);

    EXPECT_EQ(
        result.intersectionIDs[0],
        "1_1");
}

TEST(AStarStrategy, LargeGrid)
{
    Graph graph =
        TestGraphBuilder::createGrid(
            20,
            20);

    RouteRequest request(
        "0_0",
        "19_19");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_TRUE(result.isValid);
}

TEST(AStarStrategy, RouteContainsNodes)
{
    Graph graph =
        TestGraphBuilder::createGrid(5,5);

    RouteRequest request(
        "0_0",
        "4_4");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_GT(
        result.intersectionIDs.size(),
        1);
}

TEST(AStarStrategy, PathLength)
{
    Graph graph =
        TestGraphBuilder::createGrid(3,3);

    RouteRequest request(
        "0_0",
        "2_2");

    AStarStrategy astar;

    Route result =
        astar.calculateRoute(
            graph,
            request);

    EXPECT_EQ(
        result.intersectionIDs.size(),
        5);
}

TEST(AStarBenchmark, CompareHeuristics)
{
    Graph graph =
        TestGraphBuilder::createGrid(
            20,
            20);

    RouteRequest request(
        "0_0",
        "19_19");

    std::vector<HeuristicType> heuristics =
    {
        HeuristicType::Zero,
        HeuristicType::Euclidean,
        HeuristicType::WeightedEuclidean
    };

    for(auto heuristic : heuristics)
    {
        AStarStrategy astar(heuristic);

        auto start =
            std::chrono::steady_clock::now();

        Route result =
            astar.calculateRoute(
                graph,
                request);

        auto end =
            std::chrono::steady_clock::now();

        auto duration =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        EXPECT_TRUE(result.isValid);

        std::cout
            << "Time: "
            << duration.count()
            << " us\n";

        std::cout
            << "Expanded Nodes: "
            << astar.getExpandedNodeCount()
            << "\n\n";
    }
}

int runAstar_Tests()
{
    int   argc    = 1;
    char  name[]  = "AStar_test";
    char* argv[]  = { name, nullptr };

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}