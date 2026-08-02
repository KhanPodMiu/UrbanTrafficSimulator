#include <gtest/gtest.h>

#include <memory>

#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStarStrategy.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/RoutingManager.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "vehicles/Vehicle.hpp"
#include "simulation/RouteRequest.hpp"

namespace
{
struct RerouteFixture
{
    Graph graph;
    std::shared_ptr<Intersection> a =
        std::make_shared<Intersection>("A", 0, 0);
    std::shared_ptr<Intersection> b =
        std::make_shared<Intersection>("B", 1000, 0);
    std::shared_ptr<Intersection> c =
        std::make_shared<Intersection>("C", 0, 1000);
    std::shared_ptr<Intersection> d =
        std::make_shared<Intersection>("D", 1000, 1000);
    std::shared_ptr<Road> ab =
        std::make_shared<Road>("AB", a.get(), b.get(), 60);
    std::shared_ptr<Road> bd =
        std::make_shared<Road>("BD", b.get(), d.get(), 60);
    std::shared_ptr<Road> ac =
        std::make_shared<Road>("AC", a.get(), c.get(), 60);
    std::shared_ptr<Road> cd =
        std::make_shared<Road>("CD", c.get(), d.get(), 60);
    std::shared_ptr<Road> bc =
        std::make_shared<Road>("BC", b.get(), c.get(), 60);
    RoutingManager routingManager{std::make_unique<Dijkstra>()};

    RerouteFixture()
    {
        graph.addIntersection(a);
        graph.addIntersection(b);
        graph.addIntersection(c);
        graph.addIntersection(d);
        graph.addRoad(ab);
        graph.addRoad(bd);
        graph.addRoad(ac);
        graph.addRoad(cd);
        graph.addRoad(bc);
    }
};
}

TEST(BannedRouteRerouteTest, MovesVehicleOffNewlyBannedCurrentRoad)
{
    RerouteFixture fixture;
    Vehicle vehicle("test-vehicle");
    vehicle.setRoute({fixture.ab, fixture.bd});
    vehicle.setCurrentRoad(fixture.ab);
    vehicle.setDestination(fixture.d);
    vehicle.setDistanceOnRoad(400.0);
    fixture.ab->vehicleEnters(&vehicle);
    fixture.ab->setVIPExclusive(true);

    ASSERT_TRUE(vehicle.rerouteAroundBannedRoads(
        fixture.graph,
        fixture.routingManager));

    ASSERT_EQ(vehicle.getCurrentRoad(), fixture.ac);
    EXPECT_DOUBLE_EQ(vehicle.getDistanceOnRoad(), 400.0);
    ASSERT_EQ(vehicle.getRoute().size(), 2U);
    EXPECT_EQ(vehicle.getRoute()[0], fixture.ac);
    EXPECT_EQ(vehicle.getRoute()[1], fixture.cd);
    EXPECT_TRUE(fixture.ab->getVehicles().empty());
    ASSERT_EQ(fixture.ac->getVehicles().size(), 1U);
    EXPECT_EQ(fixture.ac->getVehicles().front(), &vehicle);
}

TEST(BannedRouteRerouteTest, EveryRoutingStrategySkipsBannedRoads)
{
    RerouteFixture fixture;
    fixture.ab->setVIPExclusive(true);
    const RouteRequest request("A", "D");
    const std::vector<std::string> expectedRoute{"A", "C", "D"};

    BFS bfs;
    Dijkstra dijkstra;
    AStarStrategy aStar;

    EXPECT_EQ(
        bfs.calculateRoute(fixture.graph, request).intersectionIDs,
        expectedRoute);
    EXPECT_EQ(
        dijkstra.calculateRoute(fixture.graph, request).intersectionIDs,
        expectedRoute);
    EXPECT_EQ(
        aStar.calculateRoute(fixture.graph, request).intersectionIDs,
        expectedRoute);
}

TEST(BannedRouteRerouteTest, ReplacesBannedFutureRoadAfterCurrentRoad)
{
    RerouteFixture fixture;
    Vehicle vehicle("test-vehicle");
    vehicle.setRoute({fixture.ab, fixture.bd});
    vehicle.setCurrentRoad(fixture.ab);
    vehicle.setDestination(fixture.d);
    vehicle.setDistanceOnRoad(400.0);
    fixture.ab->vehicleEnters(&vehicle);
    fixture.bd->setVIPExclusive(true);

    ASSERT_TRUE(vehicle.rerouteAroundBannedRoads(
        fixture.graph,
        fixture.routingManager));

    EXPECT_EQ(vehicle.getCurrentRoad(), fixture.ab);
    EXPECT_DOUBLE_EQ(vehicle.getDistanceOnRoad(), 400.0);
    ASSERT_EQ(vehicle.getRoute().size(), 3U);
    EXPECT_EQ(vehicle.getRoute()[0], fixture.ab);
    EXPECT_EQ(vehicle.getRoute()[1], fixture.bc);
    EXPECT_EQ(vehicle.getRoute()[2], fixture.cd);
}
