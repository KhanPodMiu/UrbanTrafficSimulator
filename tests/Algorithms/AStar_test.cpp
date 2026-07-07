#include <gtest/gtest.h>

#include "algorithms/AStarStrategy.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"
#include "algorithms/BFS.hpp"
#include "utils/MapLoader.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>
#include <filesystem>

TEST(MapTest, MapFileExists)
{
    EXPECT_TRUE(
        std::filesystem::exists(
            "../assets/maps/DinhDocLap.json"
        )
    );
}
// const std::string MAP_PATH ="../assets/maps/small_map.json";

class AStarTest : public ::testing::Test
{
protected:
    Graph graph;

    void SetUp() override
    {
        ASSERT_TRUE(
            MapLoader::loadFromJson(
                "../assets/maps/DinhDocLap.json",
                graph
            )
        );
    }
};

TEST_F(AStarTest, ValidRoute)
{
    AStarStrategy astar;

    RouteRequest request("I1", "I100");

    RouteResult result =
        astar.calculateRoute(graph, request);

    EXPECT_TRUE(result.isSuccess);
    EXPECT_FALSE(result.intersectionIDs.empty());
}

// TEST_F(AStarTest, SameSourceDestination)
// {
//     AStarStrategy astar;

//     RouteRequest request("I1", "I1");

//     RouteResult result =
//         astar.calculateRoute(graph, request);

//     EXPECT_TRUE(result.isSuccess);

//     EXPECT_EQ(
//         result.intersectionIDs.front(),
//         "I1"
//     );

//     EXPECT_EQ(
//         result.intersectionIDs.back(),
//         "I1"
//     );
// }

TEST_F(AStarTest, BenchmarkHeuristics)
{
    RouteRequest request("I1", "I414");

    std::vector<std::pair<std::string, HeuristicType>>
    heuristics =
    {
        {"Zero", HeuristicType::Zero},
        {"Euclidean", HeuristicType::Euclidean},
        {"Weighted", HeuristicType::WeightedEuclidean},
        {"Divide Speed", HeuristicType::DivideSpeed}
    };

    std::cout << "\n";
    std::cout << "=====================================\n";
    std::cout << "A* Heuristic Benchmark\n";
    std::cout << "=====================================\n";

    for(const auto& h : heuristics)
    {
        AStarStrategy astar(h.second);

        auto start =
            std::chrono::high_resolution_clock::now();

        RouteResult result =
            astar.calculateRoute(graph, request);

        auto stop =
            std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    stop - start);

        std::cout
            << std::setw(12)
            << h.first
            << " | Time: "
            << std::setw(6)
            << duration.count()
            << " us | Expanded: "
            << astar.getExpandedNodeCount()
            << " | Path Length: "
            << result.intersectionIDs.size()
            << "\n"
            << "Travel Cost: "
            << astar.getLastTravelCost()
            << '\n';
    }
}

TEST_F(AStarTest, BenchmarkBFS)
{
    RouteRequest request("I1", "I414");

    auto start =
        std::chrono::high_resolution_clock::now();

    RouteResult result =
        BFS::findShortestPath(graph, request);

    auto stop =
        std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<
            std::chrono::microseconds>(
                stop - start);

    std::cout << "\n";
    // std::cout << "=====================================\n";
    // std::cout << "BFS Benchmark\n";
    // std::cout << "=====================================\n";

    std::cout
        << "Time: "
        << std::setw(6)
        << duration.count()
        << " us | Expanded: "
        << BFS::getExpandedNodeCount()
        << " | Path Length: "
        << result.intersectionIDs.size()
        << "\n";
}

int runAstar_Tests()
{
    int   argc    = 1;
    char  name[]  = "AStar_test";
    char* argv[]  = { name, nullptr };

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}