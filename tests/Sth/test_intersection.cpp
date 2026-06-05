// test_intersection.cpp

#include <gtest/gtest.h>
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"


// --------CONSTRUCTOR TEST---------------
TEST(INTERSECTION, ValidData) {
    Intersection intersection(
            "I1",
            100,
            200,
            IntersectionType::CROSS
    );
    EXPECT_EQ(intersection.getIntersectionID(), "I1");
    EXPECT_EQ(intersection.getX(), 100);
    EXPECT_EQ(intersection.getY(), 200);
    EXPECT_EQ(intersection.getType(),
              IntersectionType::CROSS);
}

TEST(IntersectionConstructorTest,
    CreateIntersection_WithXAtLowerBoundary)
{
    EXPECT_NO_THROW(
        Intersection(
            "I1",
            0,
            100,
            IntersectionType::CROSS));
}

TEST(IntersectionConstructorTest,
    CreateIntersection_WithXAtUpperBoundary)
{
    EXPECT_NO_THROW(
        Intersection(
            "I1",
            4000,
            100,
            IntersectionType::CROSS));
}

TEST(IntersectionConstructorTest,
    CreateIntersection_WithXBelowLowerBoundary)
{
    EXPECT_THROW(
        Intersection(
            "I1",
            -1,
            100,
            IntersectionType::CROSS),
        std::invalid_argument);
}

TEST(IntersectionConstructorTest,
    CreateIntersection_WithXAboveUpperBoundary)
{
    EXPECT_THROW(
        Intersection(
            "I1",
            4001,
            100,
            IntersectionType::CROSS),
        std::invalid_argument);
}

//------------GETTER TEST------------
TEST(IntersectionGetterTest,ValidGetter)
{
    Intersection intersection(
        "I1",
        100,
        200,
        IntersectionType::ROUNDABOUT);

    EXPECT_EQ(
        intersection.getIntersectionID(),"I1");
    EXPECT_EQ(
        intersection.getX(),100);
    EXPECT_EQ(
        intersection.getY(),200);
    EXPECT_EQ(
        intersection.getType(),
        IntersectionType::ROUNDABOUT);
}

TEST(IntersectionGetterTest,Empty_State)
{
    Intersection intersection(
        "I1",
        100,
        200,
        IntersectionType::CROSS);

    EXPECT_TRUE(
        intersection.getIncomingRoads().empty());

    EXPECT_TRUE(
        intersection.getOutgoingRoads().empty());

    
    EXPECT_EQ(
        intersection.getIncomingRoadCount(),0);

    EXPECT_EQ(
        intersection.getOutgoingRoadCount(),0);

    EXPECT_EQ(
        intersection.getDegree(),0);
}

TEST(IntersectionGetterTest,
     GetIncomingRoadCount_ReturnsCorrectCount)
{
    Intersection source(
        "S",
        100,
        100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",
        200,
        200,
        IntersectionType::CROSS);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    destination.addIncomingRoad(&road);

    EXPECT_EQ(
        destination.getIncomingRoadCount(),
        1);
}

TEST(IntersectionGetterTest,
     GetDegree_ReturnsSumOfIncomingAndOutgoingRoads)
{
    Intersection center(
        "C",
        100,
        100,
        IntersectionType::CROSS);

    Intersection a(
        "A",
        0,
        0,
        IntersectionType::CROSS);

    Intersection b(
        "B",
        0,
        0,
        IntersectionType::CROSS);

    Road incoming(
        "R1",
        &a,
        &center,
        100,
        60);

    Road outgoing(
        "R2",
        &center,
        &b,
        100,
        60);

    center.addIncomingRoad(&incoming);
    center.addOutgoingRoad(&outgoing);

    EXPECT_EQ(
        center.getDegree(),
        2);
}

//----------- Setter TEST ---------
TEST(IntersectionSetterTest,
     AddIncomingRoad_AddsRoadSuccessfully)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

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

TEST(IntersectionSetterTest,
     AddIncomingRoad_DoesNotAddDuplicateRoad)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

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

TEST(IntersectionSetterTest,
     AddIncomingRoad_RejectsNullptr)
{
    Intersection intersection(
        "I1",
        100,
        100,
        IntersectionType::CROSS);

    EXPECT_FALSE(
        intersection.addIncomingRoad(nullptr));

    EXPECT_EQ(
        intersection.getIncomingRoadCount(),
        0);
}

TEST(IntersectionSetterTest,
     AddOutgoingRoad_AddsRoadSuccessfully)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

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

//--------- REMOVE TEST---------
TEST(IntersectionRemoveTest,
     RemoveIncomingRoad_RemovesExistingRoad)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

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

TEST(IntersectionRemoveTest,
     RemoveIncomingRoad_ReturnsFalseWhenRoadNotFound)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    EXPECT_FALSE(
        destination.removeIncomingRoad(&road));

    EXPECT_EQ(
        destination.getIncomingRoadCount(),
        0);
}

TEST(IntersectionRemoveTest,
     RemoveIncomingRoad_RejectsNullptr)
{
    Intersection intersection(
        "I1",
        100,
        100,
        IntersectionType::CROSS);

    EXPECT_FALSE(
        intersection.removeIncomingRoad(nullptr));
}

TEST(IntersectionRemoveTest,
     RemoveIncomingRoad_RemovesCorrectRoad)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

    Road road1(
        "R1",
        &source,
        &destination,
        100,
        60);

    Road road2(
        "R2",
        &source,
        &destination,
        200,
        60);

    destination.addIncomingRoad(&road1);
    destination.addIncomingRoad(&road2);

    EXPECT_TRUE(
        destination.removeIncomingRoad(&road2));

    EXPECT_EQ(
        destination.getIncomingRoadCount(),
        1);

    EXPECT_EQ(
        destination.getIncomingRoads()[0],
        &road1);
}

TEST(IntersectionRemoveTest,
     RemoveOutgoingRoad_RemovesExistingRoad)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

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

TEST(IntersectionRemoveTest,
     RemoveOutgoingRoad_ReturnsFalseWhenRoadNotFound)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

    Road road(
        "R1",
        &source,
        &destination,
        100,
        60);

    EXPECT_FALSE(
        source.removeOutgoingRoad(&road));
}

TEST(IntersectionRemoveTest,
     RemoveOutgoingRoad_RejectsNullptr)
{
    Intersection intersection(
        "I1",
        100,
        100,
        IntersectionType::CROSS);

    EXPECT_FALSE(
        intersection.removeOutgoingRoad(nullptr));
}

TEST(IntersectionRemoveTest,
     RemoveOutgoingRoad_RemovesCorrectRoad)
{
    Intersection source(
        "S",100,100,
        IntersectionType::CROSS);

    Intersection destination(
        "D",200,200,
        IntersectionType::CROSS);

    Road road1(
        "R1",
        &source,
        &destination,
        100,
        60);

    Road road2(
        "R2",
        &source,
        &destination,
        200,
        60);

    source.addOutgoingRoad(&road1);
    source.addOutgoingRoad(&road2);

    EXPECT_TRUE(
        source.removeOutgoingRoad(&road2));

    EXPECT_EQ(
        source.getOutgoingRoadCount(),
        1);

    EXPECT_EQ(
        source.getOutgoingRoads()[0],
        &road1);
}

int runIntersectionTests()
{
    int   argc    = 1;
    char  name[]  = "Intersection_test";
    char* argv[]  = { name, nullptr };

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}