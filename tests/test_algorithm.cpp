#include <gtest/gtest.h>
#include <memory>
#include "Graph.hpp"
#include "Intersection.hpp"
#include "Road.hpp"
#include "BFS.hpp"       // Nhớ include file thuật toán của bạn
#include "Dijkstra.hpp"  // Nhớ include file thuật toán của bạn
#include "RouteRequest.hpp"
#include "RouteResult.hpp"


Graph createTestGraph() {
    Graph g;

    // 1. KHỞI TẠO INTERSECTION (Nút giao)
    // Constructor: Intersection(id, x, y)
    auto intA = std::make_shared<Intersection>("A", 0, 0);
    auto intB = std::make_shared<Intersection>("B", 100, 0);
    auto intC = std::make_shared<Intersection>("C", 0, 100);
    auto intD = std::make_shared<Intersection>("D", 100, 100);
    auto intX = std::make_shared<Intersection>("X", 500, 500); 

    g.addIntersection(intA);
    g.addIntersection(intB);
    g.addIntersection(intC);
    g.addIntersection(intD);
    g.addIntersection(intX);

    auto roadAB = std::make_shared<Road>("R_AB", intA.get(), intB.get(), 10, 50, 0); 
    auto roadAC = std::make_shared<Road>("R_AC", intA.get(), intC.get(), 2, 50, 0);
    auto roadCD = std::make_shared<Road>("R_CD", intC.get(), intD.get(), 2, 50, 0);
    auto roadDB = std::make_shared<Road>("R_DB", intD.get(), intB.get(), 2, 50, 0);

    g.addRoad(roadAB);
    g.addRoad(roadAC);
    g.addRoad(roadCD);
    g.addRoad(roadDB);

    return g;
}

// ================= TEST CASES CHO BFS =================

TEST(BFSTest, TimDuongItCanhNhat) {
    Graph g = createTestGraph();
    RouteRequest req("A", "B");

    RouteResult result = BFS::findShortestPath(g, req);

    EXPECT_TRUE(result.isSuccess);
    ASSERT_EQ(result.intersectionIDs.size(), 2); 
    EXPECT_EQ(result.intersectionIDs[0], "A");
    EXPECT_EQ(result.intersectionIDs[1], "B");
}

TEST(BFSTest, KhongTimThayDuong_NgoCut) {
    Graph g = createTestGraph();
    RouteRequest req("A", "X"); // X là hòn đảo tách biệt

    RouteResult result = BFS::findShortestPath(g, req);

    EXPECT_FALSE(result.isSuccess);
    EXPECT_TRUE(result.intersectionIDs.empty());
}

// ================= TEST CASES CHO DIJKSTRA =================

TEST(DijkstraTest, TimDuongChiPhiReNhat) {
    Graph g = createTestGraph();
    RouteRequest req("A", "B");

    RouteResult result = Dijkstra::findShortestPath(g, req);

    EXPECT_TRUE(result.isSuccess);
    ASSERT_EQ(result.intersectionIDs.size(), 4); 
    EXPECT_EQ(result.intersectionIDs[0], "A");
    EXPECT_EQ(result.intersectionIDs[1], "C");
    EXPECT_EQ(result.intersectionIDs[2], "D");
    EXPECT_EQ(result.intersectionIDs[3], "B");
}

TEST(DijkstraTest, CungDiemXuatPhat) {
    Graph g = createTestGraph();
    RouteRequest req("A", "A"); // Start == Dest

    RouteResult result = Dijkstra::findShortestPath(g, req);

    EXPECT_TRUE(result.isSuccess);
    ASSERT_EQ(result.intersectionIDs.size(), 1);
    EXPECT_EQ(result.intersectionIDs[0], "A");
}