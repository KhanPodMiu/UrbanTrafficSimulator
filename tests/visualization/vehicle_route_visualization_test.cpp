#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "vehicles/Car.hpp"

TEST(VehicleRouteVisualizationTest, VehicleStoresAndExposesRouteForVisualization)
{
    auto i1 = std::make_shared<Intersection>("I1", 100, 100);
    auto i2 = std::make_shared<Intersection>("I2", 500, 100);
    auto i3 = std::make_shared<Intersection>("I3", 500, 500);

    auto road1 = std::make_shared<Road>("R1", i1.get(), i2.get(), 60);
    auto road2 = std::make_shared<Road>("R2", i2.get(), i3.get(), 60);

    Car car("C1");
    std::vector<std::shared_ptr<Road>> route = {road1, road2};
    car.setRoute(route);
    car.setCurrentRoad(road1);
    car.setDestination(i3);

    EXPECT_EQ(car.getRoute().size(), 2U);
    EXPECT_EQ(car.getRouteIndex(), 0U);
    EXPECT_EQ(car.getRoute()[0]->getSourceIntersection()->getIntersectionID(), "I1");
    EXPECT_EQ(car.getRoute()[0]->getDestinationIntersection()->getIntersectionID(), "I2");
    EXPECT_EQ(car.getRoute()[1]->getDestinationIntersection()->getIntersectionID(), "I3");

    // Advance route index
    car.setRouteIndex(1);
    EXPECT_EQ(car.getRouteIndex(), 1U);
}
