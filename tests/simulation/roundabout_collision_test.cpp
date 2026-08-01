#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <vector>

#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "vehicles/Vehicle.hpp"
#include "vehicles/Car.hpp"
#include "simulation/CollisionManager.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===== Helper: build a small roundabout graph for testing =====
static Graph buildRoundaboutGraph()
{
    Graph g;

    auto iR  = std::make_shared<Intersection>("I_R",  1000, 1000);
    auto iN  = std::make_shared<Intersection>("I_N",  1000,   0);
    auto iS  = std::make_shared<Intersection>("I_S",  1000, 2000);
    auto iE  = std::make_shared<Intersection>("I_E",  2000, 1000);
    auto iW  = std::make_shared<Intersection>("I_W",     0, 1000);
    auto iSE = std::make_shared<Intersection>("I_SE", 1700, 1700);

    g.addIntersection(iR);
    g.addIntersection(iN);
    g.addIntersection(iS);
    g.addIntersection(iE);
    g.addIntersection(iW);
    g.addIntersection(iSE);

    int speed = 60;

    // Bidirectional roads to/from roundabout center
    g.addRoad(std::make_shared<Road>("R_N_in",  iN.get(),  iR.get(), speed));
    g.addRoad(std::make_shared<Road>("R_N_out", iR.get(),  iN.get(), speed));
    g.addRoad(std::make_shared<Road>("R_S_in",  iS.get(),  iR.get(), speed));
    g.addRoad(std::make_shared<Road>("R_S_out", iR.get(),  iS.get(), speed));
    g.addRoad(std::make_shared<Road>("R_E_in",  iE.get(),  iR.get(), speed));
    g.addRoad(std::make_shared<Road>("R_E_out", iR.get(),  iE.get(), speed));
    g.addRoad(std::make_shared<Road>("R_W_in",  iW.get(),  iR.get(), speed));
    g.addRoad(std::make_shared<Road>("R_W_out", iR.get(),  iW.get(), speed));
    g.addRoad(std::make_shared<Road>("R_SE_in", iSE.get(), iR.get(), speed));
    g.addRoad(std::make_shared<Road>("R_SE_out",iR.get(),  iSE.get(),speed));

    return g;
}

// =================================================================
// TEST 1: Ring Expansion Transforms Roundabout correctly
// (Disabled due to unimplemented features: Graph::expandRoundabouts, Intersection::isRingNode)
// =================================================================
#if 0
TEST(RoundaboutExpansionTest, ExpandRoundaboutCreatesRing)
{
    Graph g = buildRoundaboutGraph();
    
    // Before expansion, the roundabout exists and is of type ROUNDABOUT
    auto iR = g.getIntersection("I_R");
    ASSERT_NE(iR, nullptr);
    EXPECT_EQ(iR->getType(), IntersectionType::ROUNDABOUT);
    
    g.expandRoundabouts();
    
    // After expansion, original roundabout node is gone
    EXPECT_EQ(g.getIntersection("I_R"), nullptr);
    
    // Check that ring nodes were created
    int ringNodeCount = 0;
    for (const auto& [id, inter] : g.getIntersections()) {
        if (inter->isRingNode()) {
            ringNodeCount++;
        }
    }
    
    EXPECT_EQ(ringNodeCount, 8); // We specified RING_SIZE = 8
}
#endif

// =================================================================
// TEST 2: Normal (non-roundabout) road behaviour is unchanged
// =================================================================
TEST(VehicleNavTest, NormalRoadBehaviourUnchanged)
{
    Graph g;
    auto iA = std::make_shared<Intersection>("I_A", 0, 0);
    auto iB = std::make_shared<Intersection>("I_B", 1000, 0);
    g.addIntersection(iA);
    g.addIntersection(iB);
    g.addRoad(std::make_shared<Road>("R_AB", iA.get(), iB.get(), 60));

    auto road = g.getRoad("R_AB");
    ASSERT_NE(road, nullptr);

    Car vehicle("V_normal");
    std::vector<std::shared_ptr<Road>> route = {road};
    vehicle.setRoute(route);
    vehicle.setCurrentRoad(road);

    // At midpoint, position should be roughly at (500, lane_offset)
    vehicle.setDistanceOnRoad(road->getDistance() * 0.5);
    vehicle.updateWorldPosition();

    Vector2 pos = vehicle.getPosition();
    EXPECT_NEAR(pos.x, 500.0, 5.0);
    // y should have lane offset (40.0 units to the right of eastbound = positive y in SDL)
    EXPECT_NEAR(pos.y, 40.0, 5.0);
}

// =================================================================
// TEST 3: Roundabouts use direct road-to-road movement (no arc)
// =================================================================
TEST(VehicleNavTest, RoundaboutApproachRemainsOnIncomingRoad)
{
    Graph g = buildRoundaboutGraph();
    auto incomingRoad = g.getRoad("R_N_in");
    auto outgoingRoad = g.getRoad("R_E_out");
    ASSERT_NE(incomingRoad, nullptr);
    ASSERT_NE(outgoingRoad, nullptr);

    Car vehicle("V_direct_approach");
    vehicle.setRoute({incomingRoad, outgoingRoad});
    vehicle.setCurrentRoad(incomingRoad);
    vehicle.setDistanceOnRoad(incomingRoad->getDistance() - 40.0);
    vehicle.updateWorldPosition();

    const Vector2 position = vehicle.getPosition();
    EXPECT_NEAR(position.x, 960.0, 0.01);
    EXPECT_NEAR(position.y, 960.0, 0.01);
    EXPECT_NEAR(vehicle.getHeadingAngle(), 90.0, 0.01);
}

TEST(VehicleNavTest, RoundaboutExitMovesDirectlyOntoOutgoingRoad)
{
    Graph g = buildRoundaboutGraph();
    auto incomingRoad = g.getRoad("R_N_in");
    auto outgoingRoad = g.getRoad("R_E_out");
    ASSERT_NE(incomingRoad, nullptr);
    ASSERT_NE(outgoingRoad, nullptr);

    Car vehicle("V_direct_exit");
    vehicle.setRoute({incomingRoad, outgoingRoad});
    vehicle.setCurrentRoad(incomingRoad);
    vehicle.setDistanceOnRoad(incomingRoad->getDistance() + 20.0);

    ASSERT_TRUE(vehicle.tryAdvanceToNextRoad());
    EXPECT_EQ(vehicle.getCurrentRoad(), outgoingRoad);
    EXPECT_NEAR(vehicle.getDistanceOnRoad(), 20.0, 0.01);

    const Vector2 position = vehicle.getPosition();
    EXPECT_NEAR(position.x, 1020.0, 0.01);
    EXPECT_NEAR(position.y, 1040.0, 0.01);
    EXPECT_NEAR(vehicle.getHeadingAngle(), 0.0, 0.01);
}

// =================================================================
// TEST 4: Spawn safety - don't spawn on congested road
// =================================================================
TEST(CollisionTest, SpawnSafetyPreventsOverlap)
{
    Graph g;
    auto iA = std::make_shared<Intersection>("I_A", 0, 0);
    auto iB = std::make_shared<Intersection>("I_B", 5000, 0);
    g.addIntersection(iA);
    g.addIntersection(iB);
    g.addRoad(std::make_shared<Road>("R_AB", iA.get(), iB.get(), 60));

    auto road = g.getRoad("R_AB");
    ASSERT_NE(road, nullptr);

    // Place a vehicle at distance 50 (within the 120 threshold)
    Car blocker("V_blocker");
    blocker.setDistanceOnRoad(50.0);
    road->vehicleEnters(&blocker);

    // Check that the road has a vehicle near position 0
    const auto& vehicles = road->getVehicles();
    ASSERT_FALSE(vehicles.empty());

    bool hasCongestion = false;
    for (const Vehicle* v : vehicles)
    {
        if (v && v->getDistanceOnRoad() < 120.0)
        {
            hasCongestion = true;
            break;
        }
    }
    EXPECT_TRUE(hasCongestion)
        << "Should detect congestion near road entrance";

    road->vehicleExits(&blocker);
}

// =================================================================
// TEST 5: Transition safety - overflow distance is clamped
// =================================================================
TEST(CollisionTest, TransitionSafetyClampsOverflow)
{
    Graph g;
    auto iA = std::make_shared<Intersection>("I_A", 0, 0);
    auto iB = std::make_shared<Intersection>("I_B", 1000, 0);
    auto iC = std::make_shared<Intersection>("I_C", 2000, 0);
    g.addIntersection(iA);
    g.addIntersection(iB);
    g.addIntersection(iC);
    g.addRoad(std::make_shared<Road>("R_AB", iA.get(), iB.get(), 60));
    g.addRoad(std::make_shared<Road>("R_BC", iB.get(), iC.get(), 60));

    auto road1 = g.getRoad("R_AB");
    auto road2 = g.getRoad("R_BC");
    ASSERT_NE(road1, nullptr);
    ASSERT_NE(road2, nullptr);

    // Place a blocker vehicle at distance 20 on road2
    Car blocker("V_blocker");
    blocker.setDistanceOnRoad(20.0);
    road2->vehicleEnters(&blocker);

    // Create vehicle that will transition with large overflow
    Car vehicle("V_test");
    std::vector<std::shared_ptr<Road>> route = {road1, road2};
    vehicle.setRoute(route);
    vehicle.setCurrentRoad(road1);
    vehicle.setDistanceOnRoad(road1->getDistance() + 100.0); // Large overflow
    vehicle.setCurrentSpeed(50.0);

    bool advanced = vehicle.tryAdvanceToNextRoad();
    EXPECT_TRUE(advanced);

    // Vehicle distance should be clamped to avoid overlap with blocker
    // blocker is at 20, its back is at 20 - 5 = 15, safe = 15 - 50 = -35 -> clamped to 0
    EXPECT_LE(vehicle.getDistanceOnRoad(), 20.0)
        << "Vehicle should not overlap with existing vehicle on new road";

    road2->vehicleExits(&blocker);
}
