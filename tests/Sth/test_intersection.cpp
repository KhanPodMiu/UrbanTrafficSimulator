// test_intersection.cpp

#include <gtest/gtest.h>
#include "graph/Intersection.hpp"

class Road {};

TEST(IntersectionTest, Constructor)
{
    Intersection intersection(
        "INT001",
        10,
        20,
        IntersectionType::CROSS);

    EXPECT_EQ(
        intersection.getIntersectionID(),
        "INT001");

    EXPECT_DOUBLE_EQ(
        intersection.getX(),
        10);

    EXPECT_DOUBLE_EQ(
        intersection.getY(),
        20);

    EXPECT_EQ(
        intersection.getType(),
        IntersectionType::CROSS);
}

TEST(IntersectionTest, AddRoad)
{
    Intersection intersection;

    Road road;

    intersection.addIncomingRoad(&road);

    EXPECT_EQ(
        intersection.getIncomingRoad().size(),
        1);
}

TEST(IntersectionTest, RemoveRoad)
{
    Intersection intersection;

    Road road;

    intersection.addIncomingRoad(&road);
    intersection.removeIncomingRoad(&road);

    EXPECT_TRUE(
        intersection.getIncomingRoad().empty());
}