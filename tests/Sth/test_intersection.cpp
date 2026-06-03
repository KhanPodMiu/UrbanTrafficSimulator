// test_intersection.cpp

#include <gtest/gtest.h>
#include "graph/Intersection.hpp"

class Road
{
    // Mock class để test
};

TEST(IntersectionTest, Constructor)
{
    Intersection intersection(
        "I1",
        100,
        200,
        IntersectionType::CROSS);

    EXPECT_EQ(intersection.getIntersectionID(), "I1");
    EXPECT_EQ(intersection.getX(), 100);
    EXPECT_EQ(intersection.getY(), 200);
    EXPECT_EQ(intersection.getType(),
              IntersectionType::CROSS);
}

TEST(IntersectionTest, SetPositionValid)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    bool result = intersection.setPosition(500, 600);

    EXPECT_TRUE(result);
    EXPECT_EQ(intersection.getX(), 500);
    EXPECT_EQ(intersection.getY(), 600);
}

TEST(IntersectionTest, SetPositionInvalid)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    bool result = intersection.setPosition(-10, 200);

    EXPECT_FALSE(result);

    EXPECT_EQ(intersection.getX(), 0);
    EXPECT_EQ(intersection.getY(), 0);
}

TEST(IntersectionTest, AddIncomingRoad)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road;

    EXPECT_TRUE(
        intersection.addIncomingRoad(&road));

    EXPECT_EQ(
        intersection.getIncomingRoadCount(),
        1);
}

TEST(IntersectionTest, AddIncomingRoadNullptr)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    EXPECT_FALSE(
        intersection.addIncomingRoad(nullptr));

    EXPECT_EQ(
        intersection.getIncomingRoadCount(),
        0);
}

TEST(IntersectionTest, AddIncomingRoadDuplicate)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road;

    EXPECT_TRUE(
        intersection.addIncomingRoad(&road));

    EXPECT_FALSE(
        intersection.addIncomingRoad(&road));

    EXPECT_EQ(
        intersection.getIncomingRoadCount(),
        1);
}

TEST(IntersectionTest, RemoveIncomingRoad)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road;

    intersection.addIncomingRoad(&road);

    EXPECT_TRUE(
        intersection.removeIncomingRoad(&road));

    EXPECT_EQ(
        intersection.getIncomingRoadCount(),
        0);
}

TEST(IntersectionTest, RemoveIncomingRoadNotFound)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road;

    EXPECT_FALSE(
        intersection.removeIncomingRoad(&road));
}

TEST(IntersectionTest, AddOutgoingRoad)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road;

    EXPECT_TRUE(
        intersection.addOutgoingRoad(&road));

    EXPECT_EQ(
        intersection.getOutgoingRoadCount(),
        1);
}

TEST(IntersectionTest, RemoveOutgoingRoad)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road;

    intersection.addOutgoingRoad(&road);

    EXPECT_TRUE(
        intersection.removeOutgoingRoad(&road));

    EXPECT_EQ(
        intersection.getOutgoingRoadCount(),
        0);
}

TEST(IntersectionTest, DegreeInitiallyZero)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    EXPECT_EQ(intersection.getDegree(), 0);
}

TEST(IntersectionTest, DegreeAfterAddingRoads)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road1;
    Road road2;
    Road road3;

    intersection.addIncomingRoad(&road1);
    intersection.addIncomingRoad(&road2);
    intersection.addOutgoingRoad(&road3);

    EXPECT_EQ(intersection.getIncomingRoadCount(), 2);
    EXPECT_EQ(intersection.getOutgoingRoadCount(), 1);

    EXPECT_EQ(intersection.getDegree(), 3);
}

TEST(IntersectionTest, DegreeAfterRemovingRoad)
{
    Intersection intersection(
        "I1",
        0,
        0,
        IntersectionType::CROSS);

    Road road1;
    Road road2;

    intersection.addIncomingRoad(&road1);
    intersection.addOutgoingRoad(&road2);

    EXPECT_EQ(intersection.getDegree(), 2);

    intersection.removeIncomingRoad(&road1);

    EXPECT_EQ(intersection.getDegree(), 1);
}