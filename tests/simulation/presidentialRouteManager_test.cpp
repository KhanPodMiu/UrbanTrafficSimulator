#include <gtest/gtest.h>

#include <memory>

#include "algorithms/Dijkstra.hpp"
#include "algorithms/RoutingManager.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "simulation/PresidentialRouteManager.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/VehicleManager.hpp"
#include "vehicles/Car.hpp"

namespace
{
struct PresidentialRouteFixture
{
    Graph graph;
    std::shared_ptr<Intersection> a =
        std::make_shared<Intersection>("A", 0, 0);
    std::shared_ptr<Intersection> b =
        std::make_shared<Intersection>("B", 1000, 0);
    std::shared_ptr<Road> targetRoad =
        std::make_shared<Road>("AB", a.get(), b.get(), 60);
    RoutingManager routingManager{std::make_unique<Dijkstra>()};
    VehicleManager vehicleManager{
        graph,
        routingManager,
        nullptr,
        0,
        1000.0};
    PresidentialRouteManager routeManager{graph, vehicleManager};

    PresidentialRouteFixture()
    {
        graph.addIntersection(a);
        graph.addIntersection(b);
        graph.addRoad(targetRoad);
        // MapLoader initially marks selected convoy roads as exclusive. The
        // controller converts them to the preparation state.
        targetRoad->setVIPExclusive(true);
    }
};
}

TEST(PresidentialRouteManagerTest, ActivatesAsSoonAsEmptyCorridorHasNoPlannedEntry)
{
    PresidentialRouteFixture fixture;

    EXPECT_EQ(fixture.routeManager.beginPreparation(), 0);
    EXPECT_EQ(
        fixture.routeManager.getState(),
        PresidentialRouteState::Preparing);
    EXPECT_TRUE(fixture.targetRoad->isVIPClosurePending());
    EXPECT_FALSE(fixture.targetRoad->isVIPExclusive());
    EXPECT_TRUE(fixture.targetRoad->isUnavailableForRouting());
    EXPECT_EQ(fixture.routeManager.getTargetRoadCount(), 1U);
    EXPECT_FALSE(fixture.routingManager.calculateRoute(
        fixture.graph,
        RouteRequest("A", "B")).isSuccess);

    EXPECT_TRUE(fixture.routeManager.update());
    EXPECT_EQ(
        fixture.routeManager.getState(),
        PresidentialRouteState::Active);
    EXPECT_FALSE(fixture.targetRoad->isVIPClosurePending());
    EXPECT_TRUE(fixture.targetRoad->isVIPExclusive());
}

TEST(PresidentialRouteManagerTest, WaitsUntilEveryTargetRoadIsClear)
{
    PresidentialRouteFixture fixture;
    Car vehicle("vehicle-in-corridor");
    vehicle.setCurrentRoad(fixture.targetRoad);
    fixture.targetRoad->vehicleEnters(&vehicle);

    fixture.routeManager.beginPreparation();
    EXPECT_EQ(fixture.routeManager.getVehiclesRemainingOnRoute(), 1);

    vehicle.setDistanceOnRoad(100.0);
    vehicle.setCurrentSpeed(60.0);
    vehicle.setTargetSpeed(60.0);
    vehicle.update(1.0);
    EXPECT_GT(vehicle.getDistanceOnRoad(), 100.0);

    EXPECT_FALSE(fixture.routeManager.update());
    EXPECT_EQ(
        fixture.routeManager.getState(),
        PresidentialRouteState::Preparing);
    EXPECT_TRUE(fixture.targetRoad->isVIPClosurePending());

    fixture.targetRoad->vehicleExits(&vehicle);
    EXPECT_EQ(fixture.routeManager.getVehiclesRemainingOnRoute(), 0);
    EXPECT_TRUE(fixture.routeManager.update());
    EXPECT_EQ(
        fixture.routeManager.getState(),
        PresidentialRouteState::Active);
}

TEST(PresidentialRouteManagerTest, WaitsWhileAnyVehicleRouteStillTargetsCorridor)
{
    Graph graph;
    auto a = std::make_shared<Intersection>("A", 0, 0);
    auto b = std::make_shared<Intersection>("B", 1000, 0);
    auto c = std::make_shared<Intersection>("C", 2000, 0);
    graph.addIntersection(a);
    graph.addIntersection(b);
    graph.addIntersection(c);

    auto currentRoad = std::make_shared<Road>(
        "AB", a.get(), b.get(), 60);
    auto targetRoad = std::make_shared<Road>(
        "BC", b.get(), c.get(), 60);
    graph.addRoad(currentRoad);
    graph.addRoad(targetRoad);

    RoutingManager routingManager{std::make_unique<Dijkstra>()};
    VehicleManager vehicleManager{
        graph,
        routingManager,
        nullptr,
        0,
        1000.0};
    PresidentialRouteManager routeManager{graph, vehicleManager};

    Car approachingVehicle("approaching-corridor");
    approachingVehicle.setRoute({currentRoad, targetRoad});
    approachingVehicle.setCurrentRoad(currentRoad);
    currentRoad->vehicleEnters(&approachingVehicle);
    targetRoad->setVIPExclusive(true);

    routeManager.beginPreparation();
    EXPECT_EQ(routeManager.getVehiclesRemainingOnRoute(), 0);
    EXPECT_EQ(routeManager.getVehiclesStillRoutedToCorridor(), 1);
    EXPECT_FALSE(routeManager.update());
    EXPECT_EQ(routeManager.getState(), PresidentialRouteState::Preparing);

    // Once the remaining plan no longer contains the corridor, activation is
    // allowed even though the vehicle continues elsewhere in the city.
    approachingVehicle.setRoute({currentRoad});
    EXPECT_EQ(routeManager.getVehiclesStillRoutedToCorridor(), 0);
    EXPECT_TRUE(routeManager.update());
    EXPECT_EQ(routeManager.getState(), PresidentialRouteState::Active);

    currentRoad->vehicleExits(&approachingVehicle);
}

TEST(PresidentialRouteManagerTest, CancelReopensPendingCorridor)
{
    PresidentialRouteFixture fixture;
    fixture.routeManager.beginPreparation();

    fixture.routeManager.cancel();

    EXPECT_EQ(
        fixture.routeManager.getState(),
        PresidentialRouteState::Inactive);
    EXPECT_FALSE(fixture.targetRoad->isVIPClosurePending());
    EXPECT_FALSE(fixture.targetRoad->isVIPExclusive());
    EXPECT_FALSE(fixture.targetRoad->isUnavailableForRouting());
    EXPECT_EQ(fixture.routeManager.getTargetRoadCount(), 0U);
}
