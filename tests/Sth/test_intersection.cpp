// test_intersection.cpp

#include <gtest/gtest.h>
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"


// --------CONSTRUCTOR TEST---------------
TEST(INTERSECTION, ValidData) {
    Intersection intersection(
            "I1",
            100,
            200
    );
    EXPECT_EQ(intersection.getIntersectionID(), "I1");
    EXPECT_EQ(intersection.getX(), 100);
    EXPECT_EQ(intersection.getY(), 200);
}

TEST(IntersectionRoadTest,
     AddIncomingRoad_AddsRoadSuccessfully)
{
    Intersection source("S", 0, 0);
    Intersection destination("D", 100, 100);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    EXPECT_TRUE(
        destination.addIncomingRoad(&road));

    EXPECT_EQ(
        destination.getIncomingRoadCount(),
        1);
}

TEST(IntersectionRoadTest,
     AddIncomingRoad_DoesNotDuplicateRoad)
{
    Intersection source("S", 0, 0);
    Intersection destination("D", 100, 100);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    destination.addIncomingRoad(&road);
    destination.addIncomingRoad(&road);

    EXPECT_EQ(
        destination.getIncomingRoadCount(),
        1);
}

TEST(IntersectionRoadTest,
     AddIncomingRoad_RejectsNullptr)
{
    Intersection intersection(
        "I1",
        100,
        100);

    EXPECT_FALSE(
        intersection.addIncomingRoad(nullptr));

    EXPECT_EQ(
        intersection.getIncomingRoadCount(),
        0);
}

TEST(IntersectionRoadTest,
     AddOutgoingRoad_AddsRoadSuccessfully)
{
    Intersection source("S", 0, 0);
    Intersection destination("D", 100, 100);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    EXPECT_TRUE(
        source.addOutgoingRoad(&road));

    EXPECT_EQ(
        source.getOutgoingRoadCount(),
        1);
}

TEST(IntersectionRoadTest,
     RemoveIncomingRoad_RemovesExistingRoad)
{
    Intersection source("S", 0, 0);
    Intersection destination("D", 100, 100);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    destination.addIncomingRoad(&road);

    EXPECT_TRUE(
        destination.removeIncomingRoad(&road));

    EXPECT_EQ(
        destination.getIncomingRoadCount(),
        0);
}

TEST(IntersectionRoadTest,
     RemoveIncomingRoad_RoadNotFound)
{
    Intersection source("S", 0, 0);
    Intersection destination("D", 100, 100);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    EXPECT_TRUE(
        destination.removeIncomingRoad(&road));
}

TEST(IntersectionRoadTest,
     RemoveOutgoingRoad_RemovesExistingRoad)
{
    Intersection source("S", 0, 0);
    Intersection destination("D", 100, 100);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    source.addOutgoingRoad(&road);

    EXPECT_TRUE(
        source.removeOutgoingRoad(&road));

    EXPECT_EQ(
        source.getOutgoingRoadCount(),
        0);
}

TEST(IntersectionTypeTest,
     GetType_ReturnsStraight)
{
    Intersection center("C", 100, 100);

    Intersection a("A", 0, 0);
    Intersection b("B", 0, 0);

    Road r1("R1", &a, &center, 100, 60);
    Road r2("R2", &b, &center, 100, 60);


    center.addIncomingRoad(&r1);
    center.addIncomingRoad(&r2);

    EXPECT_EQ(
        center.getType(),
        IntersectionType::STRAIGHT);
}

TEST(IntersectionTypeTest,
     GetType_ReturnsTIntersection)
{
    Intersection center("C", 100, 100);

    Intersection a("A", 0, 0);
    Intersection b("B", 0, 0);
    Intersection c("D", 0, 0);

    Road r1("R1", &a, &center, 100, 60);
    Road r2("R2", &b, &center, 100, 60);
    Road r3("R3", &center, &c, 100, 60);

    center.addIncomingRoad(&r1);
    center.addIncomingRoad(&r2);
    center.addOutgoingRoad(&r3);

    EXPECT_EQ(
        center.getType(),
        IntersectionType::T_INTERSECTION);
}

TEST(IntersectionTypeTest,
     GetType_ReturnsCross)
{
    Intersection center("C", 100, 100);

    Intersection a("A", 0, 0);
    Intersection b("B", 0, 0);
    Intersection c("D", 0, 0);
    Intersection d("E", 0, 0);

    Road r1("R1", &a, &center, 100, 60);
    Road r2("R2", &b, &center, 100, 60);
    Road r3("R3", &center, &c, 100, 60);
    Road r4("R4", &center, &d, 100, 60);

    center.addIncomingRoad(&r1);
    center.addIncomingRoad(&r2);
    center.addOutgoingRoad(&r3);
    center.addOutgoingRoad(&r4);

    EXPECT_EQ(
        center.getType(),
        IntersectionType::CROSS);
}

TEST(IntersectionTypeTest,
     GetType_ReturnsRoundabout)
{
    Intersection center("C", 100, 100);

    Intersection a("A", 0, 0);
    Intersection b("B", 0, 0);
    Intersection c("D", 0, 0);
    Intersection d("E", 0, 0);
    Intersection e("F", 0, 0);

    Road r1("R1", &a, &center, 100, 60);
    Road r2("R2", &b, &center, 100, 60);
    Road r3("R3", &c, &center, 100, 60);
    Road r4("R4", &center, &d, 100, 60);
    Road r5("R5", &center, &e, 100, 60);

    center.addIncomingRoad(&r1);
    center.addIncomingRoad(&r2);
    center.addIncomingRoad(&r3);
    center.addOutgoingRoad(&r4);
    center.addOutgoingRoad(&r5);

    EXPECT_EQ(
        center.getType(),
        IntersectionType::ROUNDABOUT);
}

TEST(IntersectionTypeTest,
     GetType_ThrowsLogicErrorForIsolatedIntersection)
{
    Intersection intersection(
        "I1",
        100,
        100);

    EXPECT_THROW(
        intersection.getType(),
        std::logic_error);
}

TEST(IntersectionTypeTest,
     GetType_ReturnsDeadEndForDegreeOne)
{
    Intersection center(
        "C",
        100,
        100);

    Intersection source(
        "S",
        0,
        0);

    Road road(
        "R1",
        &source,
        &center,
        100,
        60);

    center.addIncomingRoad(&road);

    EXPECT_EQ(
        center.getDegree(),
        1);

    EXPECT_EQ(
        center.getType(),
        IntersectionType::DEAD_END);
}

TEST(IntersectionTypeTest,
     GetType_ReturnsDeadEndForSingleOutgoingRoad)
{
    Intersection center(
        "C",
        100,
        100);

    Intersection destination(
        "D",
        200,
        200);

    Road road(
        "R1",
        &center,
        &destination,
        100,
        60);

    center.addOutgoingRoad(&road);

    EXPECT_EQ(
        center.getDegree(),
        1);

    EXPECT_EQ(
        center.getType(),
        IntersectionType::DEAD_END);
}

int runIntersectionTests()
{
    int   argc    = 1;
    char  name[]  = "Intersection_test";
    char* argv[]  = { name, nullptr };

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}