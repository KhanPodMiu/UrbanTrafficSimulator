// // tests/BFSTest.cpp

// #include <gtest/gtest.h>

// #include "algorithms/BFS.hpp"
// #include "graph/Graph.hpp"
// #include "graph/Intersection.hpp"
// #include "graph/Road.hpp"
// #include "TestGraphBuilder.hpp"
// #include "simulation/RouteRequest.hpp"
// #include "simulation/RouteResult.hpp"

// TEST(BFS, SameStartAndDestination)
// {
//     Graph graph =
//         TestGraphBuilder::createGrid(3, 3);

//     RouteRequest request("0_0", "0_0");

//     RouteResult result =
//         BFS::findShortestPath(graph, request);

//     EXPECT_TRUE(result.isSuccess);

//     ASSERT_EQ(result.intersectionIDs.size(), 1);

//     EXPECT_EQ(result.intersectionIDs[0], "0_0");
// }

// TEST(BFS, FindPathInGrid)
// {
//     Graph graph =
//         TestGraphBuilder::createGrid(3, 3);

//     RouteRequest request("0_0", "2_2");

//     RouteResult result =
//         BFS::findShortestPath(graph, request);

//     EXPECT_TRUE(result.isSuccess);

//     EXPECT_FALSE(result.intersectionIDs.empty());

//     EXPECT_EQ(
//         result.intersectionIDs.front(),
//         "0_0"
//     );

//     EXPECT_EQ(
//         result.intersectionIDs.back(),
//         "2_2"
//     );
// }

// TEST(BFS, NoPathExists)
// {
//     Graph graph;

//     auto A =
//         std::make_shared<Intersection>("A", 0, 0);

//     auto B =
//         std::make_shared<Intersection>("B", 100, 0);

//     graph.addIntersection(A);
//     graph.addIntersection(B);

//     RouteRequest request("A", "B");

//     RouteResult result =
//         BFS::findShortestPath(graph, request);

//     EXPECT_FALSE(result.isSuccess);

//     EXPECT_TRUE(result.intersectionIDs.empty());
// }

// TEST(BFS, LargeGrid)
// {
//     Graph graph =
//         TestGraphBuilder::createGrid(10, 10);

//     RouteRequest request("0_0", "9_9");

//     RouteResult result =
//         BFS::findShortestPath(graph, request);

//     EXPECT_TRUE(result.isSuccess);

//     EXPECT_EQ(
//         result.intersectionIDs.front(),
//         "0_0"
//     );

//     EXPECT_EQ(
//         result.intersectionIDs.back(),
//         "9_9"
//     );
// }

// int runBFS_Tests()
// {
//     int   argc    = 1;
//     char  name[]  = "BFS_test";
//     char* argv[]  = { name, nullptr };

//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }