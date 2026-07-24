#include <gtest/gtest.h>
#include "simulation/CollisionManager.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include "vehicles/Vehicle.hpp"

TEST(CollisionManagerTest, NormalStoppingAndCommitment)
{
    // 1. Setup Intersections
    auto startIntersection = std::make_shared<Intersection>("I1", 0, 0);
    auto endIntersection = std::make_shared<Intersection>("I2", 1000, 0);

    // 2. Setup Road
    // Speed limit: 60
    auto road = std::make_shared<Road>("R1", startIntersection.get(), endIntersection.get(), 60);
    road->setDistance(1000); // 1000 units long (stop line is at 1000 - 200 = 800)
    road->setTrafficLightEnabled(true);
    road->setTrafficLightState(TrafficLightState::RED);

    // 3. Setup Graph
    Graph graph;
    graph.addIntersection(startIntersection);
    graph.addIntersection(endIntersection);
    graph.addRoad(road);

    // 4. Create Vehicle that can stop in time
    // Stop line at 800. Vehicle at 760. Remaining distance = 40 (which is <= STOP_REACTION_DISTANCE which is 120).
    // Speed is 10. Stopping distance = (10*10)/(2*40) = 1.25. (Using EMERGENCY_DECELERATION=40)
    // Since 40 > 1.25, it should decelerate without committing.
    // The stop line speed limit at 40 is sqrt(2 * 30 * 40) = sqrt(2400) approx 48.99.
    // Since 48.99 < 60, its target speed should be reduced to 48.99.
    auto vehicleStop = std::make_shared<Vehicle>("V_STOP", 60.0);
    vehicleStop->setCurrentRoad(road);
    vehicleStop->setDistanceOnRoad(760.0);
    vehicleStop->setCurrentSpeed(10.0);
    vehicleStop->setTargetSpeed(60.0);
    road->vehicleEnters(vehicleStop.get());

    CollisionManager manager;
    manager.update(graph, 0.1);

    // It should NOT commit to intersection and target speed should be limited below 60
    EXPECT_FALSE(vehicleStop->isCommittedToIntersection());
    EXPECT_LT(vehicleStop->getTargetSpeed(), 60.0);
    EXPECT_NEAR(vehicleStop->getTargetSpeed(), 48.98979, 0.001);

    road->vehicleExits(vehicleStop.get());

    // 5. Create Vehicle that CANNOT stop in time (too fast/too close)
    // Stop line at 800. Vehicle at 796. Distance to stop line = 4. Speed = 20.0.
    // Stopping distance = (20*20)/(2*40) = 5.0.
    // Since 4 < 5.0, it cannot stop in time and should commit to the intersection.
    auto vehicleCommit = std::make_shared<Vehicle>("V_COMMIT", 60.0);
    vehicleCommit->setCurrentRoad(road);
    vehicleCommit->setDistanceOnRoad(796.0); // 4 units away from stop line (800)
    vehicleCommit->setCurrentSpeed(20.0);
    vehicleCommit->setTargetSpeed(20.0);
    road->vehicleEnters(vehicleCommit.get());

    manager.update(graph, 0.1);

    // It should commit to the intersection and target speed should be speed limit (60), not stop line limited
    EXPECT_TRUE(vehicleCommit->isCommittedToIntersection());
    EXPECT_EQ(vehicleCommit->getTargetSpeed(), 60.0);

    // 6. Test tryAdvanceToNextRoad with committed status
    // If we are committed, tryAdvanceToNextRoad should return true even if light is red and speed is low
    vehicleCommit->setCurrentSpeed(0.0);
    vehicleCommit->setDistanceOnRoad(1000.0);
    EXPECT_TRUE(vehicleCommit->tryAdvanceToNextRoad());
}
