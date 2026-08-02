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

TEST(BannedRouteRerouteTest, EvacuatesCurrentBannedRoadBeforeTakingDetour)
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

    // The vehicle must remain physically on AB until it reaches B. It must not
    // teleport to AC just because both roads share the same source.
    ASSERT_EQ(vehicle.getCurrentRoad(), fixture.ab);
    EXPECT_DOUBLE_EQ(vehicle.getDistanceOnRoad(), 400.0);
    ASSERT_EQ(vehicle.getRoute().size(), 2U);
    EXPECT_EQ(vehicle.getRoute()[0], fixture.ab);
    EXPECT_EQ(vehicle.getRoute()[1], fixture.bd);
    ASSERT_EQ(fixture.ab->getVehicles().size(), 1U);
    EXPECT_EQ(fixture.ab->getVehicles().front(), &vehicle);
    EXPECT_TRUE(fixture.ac->getVehicles().empty());

    // A banned current road no longer freezes the vehicle.
    vehicle.setCurrentSpeed(60.0);
    vehicle.setTargetSpeed(60.0);
    vehicle.update(1.0);
    EXPECT_GT(vehicle.getDistanceOnRoad(), 400.0);

    // It changes roads only after physically reaching the forward endpoint.
    vehicle.setDistanceOnRoad(fixture.ab->getDistance() + 20.0);
    ASSERT_TRUE(vehicle.tryAdvanceToNextRoad());
    EXPECT_EQ(vehicle.getCurrentRoad(), fixture.bd);
    EXPECT_DOUBLE_EQ(vehicle.getDistanceOnRoad(), 20.0);
    EXPECT_TRUE(fixture.ab->getVehicles().empty());
    ASSERT_EQ(fixture.bd->getVehicles().size(), 1U);
    EXPECT_EQ(fixture.bd->getVehicles().front(), &vehicle);
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

TEST(BannedRouteRerouteTest, EvacuatesCurrentRoadWhenDestinationHasNoDetour)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    auto destination = std::make_shared<Intersection>("D", 2000, 0);
    graph.addIntersection(a);
    graph.addIntersection(b);
    graph.addIntersection(destination);

    auto bannedRoad = std::make_shared<Road>(
        "AB", a.get(), b.get(), 60);
    graph.addRoad(bannedRoad);
    bannedRoad->setVIPExclusive(true);

    RoutingManager routingManager{std::make_unique<Dijkstra>()};
    Vehicle vehicle("evacuation-only");
    vehicle.setRoute({bannedRoad});
    vehicle.setCurrentRoad(bannedRoad);
    vehicle.setDestination(destination);
    vehicle.setDistanceOnRoad(500.0);
    bannedRoad->vehicleEnters(&vehicle);

    ASSERT_TRUE(vehicle.rerouteAroundBannedRoads(
        graph,
        routingManager));
    ASSERT_EQ(vehicle.getRoute().size(), 1U);
    EXPECT_EQ(vehicle.getCurrentRoad(), bannedRoad);

    vehicle.setCurrentSpeed(60.0);
    vehicle.setTargetSpeed(60.0);
    vehicle.update(1.0);
    EXPECT_GT(vehicle.getDistanceOnRoad(), 500.0);

    vehicle.setDistanceOnRoad(bannedRoad->getDistance());
    ASSERT_TRUE(vehicle.tryAdvanceToNextRoad());
    EXPECT_TRUE(vehicle.isFinished());
    EXPECT_TRUE(bannedRoad->getVehicles().empty());
}

TEST(BannedRouteRerouteTest, NeverEntersBannedNextRoadWithoutDetour)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    auto d = std::make_shared<Intersection>("D", 2000, 0);
    graph.addIntersection(a);
    graph.addIntersection(b);
    graph.addIntersection(d);

    auto currentRoad = std::make_shared<Road>(
        "AB", a.get(), b.get(), 60);
    auto bannedNextRoad = std::make_shared<Road>(
        "BD", b.get(), d.get(), 60);
    graph.addRoad(currentRoad);
    graph.addRoad(bannedNextRoad);
    bannedNextRoad->setVIPExclusive(true);

    Vehicle vehicle("blocked-before-ban");
    vehicle.setRoute({currentRoad, bannedNextRoad});
    vehicle.setCurrentRoad(currentRoad);
    vehicle.setDestination(d);
    vehicle.setDistanceOnRoad(currentRoad->getDistance());
    currentRoad->vehicleEnters(&vehicle);

    EXPECT_FALSE(vehicle.tryAdvanceToNextRoad());
    EXPECT_EQ(vehicle.getCurrentRoad(), currentRoad);
    EXPECT_DOUBLE_EQ(
        vehicle.getDistanceOnRoad(),
        currentRoad->getDistance());
    EXPECT_TRUE(bannedNextRoad->getVehicles().empty());
}

TEST(BannedRouteRerouteTest, RemovesUnreachableClosureFromRemainingPlan)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    auto d = std::make_shared<Intersection>("D", 2000, 0);
    graph.addIntersection(a);
    graph.addIntersection(b);
    graph.addIntersection(d);

    auto currentRoad = std::make_shared<Road>(
        "AB", a.get(), b.get(), 60);
    auto pendingRoad = std::make_shared<Road>(
        "BD", b.get(), d.get(), 60);
    graph.addRoad(currentRoad);
    graph.addRoad(pendingRoad);
    pendingRoad->setVIPClosurePending(true);

    RoutingManager routingManager{std::make_unique<Dijkstra>()};
    Vehicle vehicle("no-detour");
    vehicle.setRoute({currentRoad, pendingRoad});
    vehicle.setCurrentRoad(currentRoad);
    vehicle.setDestination(d);
    vehicle.setDistanceOnRoad(500.0);
    currentRoad->vehicleEnters(&vehicle);

    ASSERT_TRUE(vehicle.rerouteAroundBannedRoads(
        graph,
        routingManager));
    ASSERT_EQ(vehicle.getRoute().size(), 1U);
    EXPECT_EQ(vehicle.getRoute()[0], currentRoad);
    EXPECT_EQ(vehicle.getCurrentRoad(), currentRoad);
    EXPECT_FALSE(vehicle.getRoute()[0]->isUnavailableForRouting());

    currentRoad->vehicleExits(&vehicle);
}

TEST(BannedRouteRerouteTest, ChoosesLowestTravelCostAfterForwardExit)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    auto slow = std::make_shared<Intersection>("SLOW", 2000, 0);
    auto fast = std::make_shared<Intersection>("FAST", 1000, 1000);
    auto d = std::make_shared<Intersection>("D", 2000, 1000);
    graph.addIntersection(a);
    graph.addIntersection(b);
    graph.addIntersection(slow);
    graph.addIntersection(fast);
    graph.addIntersection(d);

    auto bannedCurrent = std::make_shared<Road>(
        "AB", a.get(), b.get(), 60);
    auto slowFirst = std::make_shared<Road>(
        "BS", b.get(), slow.get(), 10);
    auto slowSecond = std::make_shared<Road>(
        "SD", slow.get(), d.get(), 10);
    auto fastFirst = std::make_shared<Road>(
        "BF", b.get(), fast.get(), 100);
    auto fastSecond = std::make_shared<Road>(
        "FD", fast.get(), d.get(), 100);
    graph.addRoad(bannedCurrent);
    graph.addRoad(slowFirst);
    graph.addRoad(slowSecond);
    graph.addRoad(fastFirst);
    graph.addRoad(fastSecond);

    Vehicle vehicle("optimal-evacuation");
    vehicle.setRoute({bannedCurrent, slowFirst, slowSecond});
    vehicle.setCurrentRoad(bannedCurrent);
    vehicle.setDestination(d);
    vehicle.setDistanceOnRoad(300.0);
    bannedCurrent->vehicleEnters(&vehicle);
    bannedCurrent->setVIPExclusive(true);

    RoutingManager routingManager{std::make_unique<Dijkstra>()};
    ASSERT_TRUE(vehicle.rerouteAroundBannedRoads(
        graph,
        routingManager));

    ASSERT_EQ(vehicle.getRoute().size(), 3U);
    EXPECT_EQ(vehicle.getRoute()[0], bannedCurrent);
    EXPECT_EQ(vehicle.getRoute()[1], fastFirst);
    EXPECT_EQ(vehicle.getRoute()[2], fastSecond);
}
