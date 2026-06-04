#include <gtest/gtest.h>

#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <stdexcept>
#include <string>
#include <type_traits>   // std::is_same (compile-time type checks)

// ─────────────────────────────────────────────────────────────────────────────
//  Fixture
//  Default Road: "R1"  distance=400  speedLimit=50  congestion=0
//                travelCost = 400 × 100 / 50 = 800
// ─────────────────────────────────────────────────────────────────────────────
class RoadTest : public ::testing::Test
{
protected:
    static constexpr int DEFAULT_DISTANCE = 400;
    static constexpr int DEFAULT_SPEED    = 50;
    static constexpr int DEFAULT_COST     = 800;  // 400×100/50

    void SetUp() override
    {
        src   = new Intersection("SRC",   100, 100, IntersectionType::CROSS);
        dst   = new Intersection("DST",   500, 100, IntersectionType::CROSS);
        extra = new Intersection("EXTRA", 300, 300, IntersectionType::T_INTERSECTION);
        road  = new Road("R1", src, dst, DEFAULT_DISTANCE, DEFAULT_SPEED);
    }

    void TearDown() override
    {
        delete road;
        delete extra;
        delete dst;
        delete src;
    }

    Intersection* src   = nullptr;
    Intersection* dst   = nullptr;
    Intersection* extra = nullptr;
    Road*         road  = nullptr;
};


// ═════════════════════════════════════════════════════════════════════════════
//  1.  CONSTRUCTOR  (TEST – no fixture needed, each case builds its own Road)
// ═════════════════════════════════════════════════════════════════════════════

TEST(RoadConstruction, StoredAttributesMatchConstructorArguments)
{
    Intersection src("A", 0,  0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);

    Road road("MY_ROAD", &src, &dst, 200, 60);

    EXPECT_EQ(road.getRoadId(),                  "MY_ROAD");
    EXPECT_EQ(road.getSourceIntersection(),      &src);
    EXPECT_EQ(road.getDestinationIntersection(), &dst);
    EXPECT_EQ(road.getDistance(),                200);
    EXPECT_EQ(road.getSpeedLimit(),              60);
}

TEST(RoadConstruction, InitialCongestionLevelIsZero)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    Road road("R", &src, &dst, 200, 60);

    EXPECT_EQ(road.getCongestionLevel(), 0);
}

TEST(RoadConstruction, TravelCostIsCalculatedImmediately)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);

    // distance=500, speed=100, congestion=0 → 500×100/100 = 500
    Road road("R", &src, &dst, 500, 100);

    EXPECT_EQ(road.getTravelCost(), 500);
}

TEST(RoadConstruction, AcceptsNullptrForSource)
{
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("R", nullptr, &dst, 100, 50));
}

TEST(RoadConstruction, AcceptsNullptrForDestination)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("R", &src, nullptr, 100, 50));
}

TEST(RoadConstruction, AcceptsEmptyStringAsId)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("", &src, &dst, 100, 50));
}

TEST(RoadConstruction, AcceptsLongStringAsId)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road(std::string(1000, 'X'), &src, &dst, 100, 50));
}

// ── Invalid distance ──────────────────────────────────────────────────────────

TEST(RoadConstruction, ThrowsOnDistanceZero)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, 0, 50), std::invalid_argument);
}

TEST(RoadConstruction, ThrowsOnNegativeDistance)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, -1, 50), std::invalid_argument);
}

TEST(RoadConstruction, ThrowsOnDistanceAboveMaximum)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, Road::MAX_DISTANCE + 1, 50), std::invalid_argument);
}

TEST(RoadConstruction, DoesNotThrowOnMinimumDistance)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("R", &src, &dst, Road::MIN_DISTANCE, 50));
}

TEST(RoadConstruction, DoesNotThrowOnMaximumDistance)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("R", &src, &dst, Road::MAX_DISTANCE, 50));
}

// ── Invalid speedLimit ────────────────────────────────────────────────────────

TEST(RoadConstruction, ThrowsOnSpeedLimitZero)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, 200, 0), std::invalid_argument);
}

TEST(RoadConstruction, ThrowsOnNegativeSpeedLimit)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, 200, -1), std::invalid_argument);
}

TEST(RoadConstruction, ThrowsOnSpeedLimitBelowMinimum)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, 200, Road::MIN_SPEED_LIMIT - 1), std::invalid_argument);
}

TEST(RoadConstruction, ThrowsOnSpeedLimitAboveMaximum)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(Road("R", &src, &dst, 200, Road::MAX_SPEED_LIMIT + 1), std::invalid_argument);
}

TEST(RoadConstruction, DoesNotThrowOnMinimumSpeedLimit)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("R", &src, &dst, 200, Road::MIN_SPEED_LIMIT));
}

TEST(RoadConstruction, DoesNotThrowOnMaximumSpeedLimit)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_NO_THROW(Road("R", &src, &dst, 200, Road::MAX_SPEED_LIMIT));
}

// ── Exception content ─────────────────────────────────────────────────────────

TEST(RoadConstruction, ExceptionMessageContainsRoadId_InvalidDistance)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    try
    {
        Road("ROAD_XYZ", &src, &dst, -999, 50);
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument& e)
    {
        EXPECT_NE(std::string(e.what()).find("ROAD_XYZ"), std::string::npos)
            << "Exception message must contain the road ID";
    }
}

TEST(RoadConstruction, ExceptionMessageContainsRoadId_InvalidSpeed)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    try
    {
        Road("ROAD_ABC", &src, &dst, 200, 999);
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument& e)
    {
        EXPECT_NE(std::string(e.what()).find("ROAD_ABC"), std::string::npos)
            << "Exception message must contain the road ID";
    }
}


// ═════════════════════════════════════════════════════════════════════════════
//  2.  PARAMETERIZED  –  invalid distance / speedLimit
//  Mỗi giá trị sai đều phải throw, không cần viết lại cùng 1 test nhiều lần.
// ═════════════════════════════════════════════════════════════════════════════

class InvalidDistanceTest : public ::testing::TestWithParam<int> {};

TEST_P(InvalidDistanceTest, ConstructorThrowsInvalidArgument)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(
        Road("R", &src, &dst, GetParam(), 50),
        std::invalid_argument
    );
}

INSTANTIATE_TEST_SUITE_P(
    OutOfRangeDistances,
    InvalidDistanceTest,
    ::testing::Values(
        0,
        -1,
        -500,
        Road::MAX_DISTANCE + 1,
        Road::MAX_DISTANCE + 1000
    )
);

// ─────────────────────────────────────────────────────────────────────────────

class InvalidSpeedLimitTest : public ::testing::TestWithParam<int> {};

TEST_P(InvalidSpeedLimitTest, ConstructorThrowsInvalidArgument)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 10, 0, IntersectionType::CROSS);
    EXPECT_THROW(
        Road("R", &src, &dst, 200, GetParam()),
        std::invalid_argument
    );
}

INSTANTIATE_TEST_SUITE_P(
    OutOfRangeSpeedLimits,
    InvalidSpeedLimitTest,
    ::testing::Values(
        0,
        -1,
        -100,
        Road::MIN_SPEED_LIMIT - 1,
        Road::MAX_SPEED_LIMIT + 1,
        Road::MAX_SPEED_LIMIT + 100
    )
);


// ═════════════════════════════════════════════════════════════════════════════
//  3.  GETTERS
// ═════════════════════════════════════════════════════════════════════════════

TEST_F(RoadTest, GetRoadId_ReturnsCorrectValue)
{
    EXPECT_EQ(road->getRoadId(), "R1");
}

TEST_F(RoadTest, GetRoadId_ReturnsByConstReference_NotCopy)
{
    // Trả value copy thì tốn memory; phải là const std::string&
    static_assert(
        std::is_same<decltype(road->getRoadId()), const std::string&>::value,
        "getRoadId() must return const std::string& to avoid copying"
    );
    SUCCEED();
}

TEST_F(RoadTest, GetSourceIntersection_ReturnsCorrectAddress)
{
    EXPECT_EQ(road->getSourceIntersection(), src);
}

TEST_F(RoadTest, GetDestinationIntersection_ReturnsCorrectAddress)
{
    EXPECT_EQ(road->getDestinationIntersection(), dst);
}

TEST_F(RoadTest, GetSourceIntersection_ReturnsConstPointer)
{
    static_assert(
        std::is_same<decltype(road->getSourceIntersection()), const Intersection*>::value,
        "getSourceIntersection() must return const Intersection* (read-only)"
    );
    SUCCEED();
}

TEST_F(RoadTest, GetDestinationIntersection_ReturnsConstPointer)
{
    static_assert(
        std::is_same<decltype(road->getDestinationIntersection()), const Intersection*>::value,
        "getDestinationIntersection() must return const Intersection* (read-only)"
    );
    SUCCEED();
}

TEST_F(RoadTest, GetDistance_ReturnsStoredValue)
{
    EXPECT_EQ(road->getDistance(), DEFAULT_DISTANCE);
}

TEST_F(RoadTest, GetSpeedLimit_ReturnsStoredValue)
{
    EXPECT_EQ(road->getSpeedLimit(), DEFAULT_SPEED);
}

TEST_F(RoadTest, GetCongestionLevel_InitiallyZero)
{
    EXPECT_EQ(road->getCongestionLevel(), 0);
}

TEST_F(RoadTest, GetTravelCost_ReturnsComputedValue)
{
    EXPECT_EQ(road->getTravelCost(), DEFAULT_COST);
}


// ═════════════════════════════════════════════════════════════════════════════
//  4.  setDistance
// ═════════════════════════════════════════════════════════════════════════════

TEST_F(RoadTest, SetDistance_ReturnsTrueOnValidInput)
{
    EXPECT_TRUE(road->setDistance(1000));
}

TEST_F(RoadTest, SetDistance_StoresNewValue)
{
    road->setDistance(1000);
    EXPECT_EQ(road->getDistance(), 1000);
}

TEST_F(RoadTest, SetDistance_RecalculatesTravelCost)
{
    road->setDistance(1000);
    // 1000 × 100 / 50 = 2000
    EXPECT_EQ(road->getTravelCost(), 2000);
}

TEST_F(RoadTest, SetDistance_ReturnsFalseOnZero)
{
    EXPECT_FALSE(road->setDistance(0));
}

TEST_F(RoadTest, SetDistance_ReturnsFalseOnNegative)
{
    EXPECT_FALSE(road->setDistance(-1));
}

TEST_F(RoadTest, SetDistance_ReturnsFalseAboveMax)
{
    EXPECT_FALSE(road->setDistance(Road::MAX_DISTANCE + 1));
}

TEST_F(RoadTest, SetDistance_PreservesAllStateOnFailure)
{
    const int prevDist = road->getDistance();
    const int prevCost = road->getTravelCost();

    road->setDistance(-999);

    EXPECT_EQ(road->getDistance(),   prevDist);
    EXPECT_EQ(road->getTravelCost(), prevCost);
}

TEST_F(RoadTest, SetDistance_BoundaryMinimum)
{
    ASSERT_TRUE(road->setDistance(Road::MIN_DISTANCE));
    EXPECT_EQ(road->getDistance(), Road::MIN_DISTANCE);
}

TEST_F(RoadTest, SetDistance_BoundaryMaximum)
{
    ASSERT_TRUE(road->setDistance(Road::MAX_DISTANCE));
    EXPECT_EQ(road->getDistance(), Road::MAX_DISTANCE);
}


// ═════════════════════════════════════════════════════════════════════════════
//  5.  setSpeedLimit
// ═════════════════════════════════════════════════════════════════════════════

TEST_F(RoadTest, SetSpeedLimit_ReturnsTrueOnValidInput)
{
    EXPECT_TRUE(road->setSpeedLimit(100));
}

TEST_F(RoadTest, SetSpeedLimit_StoresNewValue)
{
    road->setSpeedLimit(100);
    EXPECT_EQ(road->getSpeedLimit(), 100);
}

TEST_F(RoadTest, SetSpeedLimit_RecalculatesTravelCost)
{
    road->setSpeedLimit(100);
    // distance=400, speed=100, congestion=0 → 400×100/100 = 400
    EXPECT_EQ(road->getTravelCost(), 400);
}

TEST_F(RoadTest, SetSpeedLimit_ReturnsFalseOnZero)
{
    EXPECT_FALSE(road->setSpeedLimit(0));
}

TEST_F(RoadTest, SetSpeedLimit_ReturnsFalseOnNegative)
{
    EXPECT_FALSE(road->setSpeedLimit(-1));
}

TEST_F(RoadTest, SetSpeedLimit_ReturnsFalseBelowMinimum)
{
    EXPECT_FALSE(road->setSpeedLimit(Road::MIN_SPEED_LIMIT - 1));
}

TEST_F(RoadTest, SetSpeedLimit_ReturnsFalseAboveMaximum)
{
    EXPECT_FALSE(road->setSpeedLimit(Road::MAX_SPEED_LIMIT + 1));
}

TEST_F(RoadTest, SetSpeedLimit_PreservesAllStateOnFailure)
{
    const int prevSpeed = road->getSpeedLimit();
    const int prevCost  = road->getTravelCost();

    road->setSpeedLimit(0);

    EXPECT_EQ(road->getSpeedLimit(), prevSpeed);
    EXPECT_EQ(road->getTravelCost(), prevCost);
}

TEST_F(RoadTest, SetSpeedLimit_BoundaryMinimum)
{
    ASSERT_TRUE(road->setSpeedLimit(Road::MIN_SPEED_LIMIT));
    EXPECT_EQ(road->getSpeedLimit(), Road::MIN_SPEED_LIMIT);
}

TEST_F(RoadTest, SetSpeedLimit_BoundaryMaximum)
{
    ASSERT_TRUE(road->setSpeedLimit(Road::MAX_SPEED_LIMIT));
    EXPECT_EQ(road->getSpeedLimit(), Road::MAX_SPEED_LIMIT);
}


// ═════════════════════════════════════════════════════════════════════════════
//  6.  setSourceIntersection / setDestinationIntersection
// ═════════════════════════════════════════════════════════════════════════════

TEST_F(RoadTest, SetSourceIntersection_ReturnsTrueOnValidPointer)
{
    EXPECT_TRUE(road->setSourceIntersection(extra));
}

TEST_F(RoadTest, SetSourceIntersection_StoresNewPointer)
{
    road->setSourceIntersection(extra);
    EXPECT_EQ(road->getSourceIntersection(), extra);
}

TEST_F(RoadTest, SetSourceIntersection_ReturnsFalseOnNullptr)
{
    EXPECT_FALSE(road->setSourceIntersection(nullptr));
}

TEST_F(RoadTest, SetSourceIntersection_PreservesPointerOnFailure)
{
    road->setSourceIntersection(nullptr);
    EXPECT_EQ(road->getSourceIntersection(), src);
}

TEST_F(RoadTest, SetSourceIntersection_DoesNotChangeTravelCost)
{
    // Cost phụ thuộc distance/speed/congestion, không phụ thuộc intersection nào
    const int costBefore = road->getTravelCost();
    road->setSourceIntersection(extra);
    EXPECT_EQ(road->getTravelCost(), costBefore);
}

TEST_F(RoadTest, SetDestinationIntersection_ReturnsTrueOnValidPointer)
{
    EXPECT_TRUE(road->setDestinationIntersection(extra));
}

TEST_F(RoadTest, SetDestinationIntersection_StoresNewPointer)
{
    road->setDestinationIntersection(extra);
    EXPECT_EQ(road->getDestinationIntersection(), extra);
}

TEST_F(RoadTest, SetDestinationIntersection_ReturnsFalseOnNullptr)
{
    EXPECT_FALSE(road->setDestinationIntersection(nullptr));
}

TEST_F(RoadTest, SetDestinationIntersection_PreservesPointerOnFailure)
{
    road->setDestinationIntersection(nullptr);
    EXPECT_EQ(road->getDestinationIntersection(), dst);
}

TEST_F(RoadTest, SetDestinationIntersection_DoesNotChangeTravelCost)
{
    const int costBefore = road->getTravelCost();
    road->setDestinationIntersection(extra);
    EXPECT_EQ(road->getTravelCost(), costBefore);
}

TEST_F(RoadTest, SetIntersection_CanSetSameNodeAsBothEndpoints)
{
    // Self-loop hợp lệ ở cấp Road; graph manager chịu trách nhiệm topology.
    EXPECT_TRUE(road->setDestinationIntersection(src));
    EXPECT_EQ(road->getSourceIntersection(),      src);
    EXPECT_EQ(road->getDestinationIntersection(), src);
}

TEST_F(RoadTest, SetIntersection_CanSwapSourceAndDestination)
{
    ASSERT_TRUE(road->setSourceIntersection(dst));
    ASSERT_TRUE(road->setDestinationIntersection(src));
    EXPECT_EQ(road->getSourceIntersection(),      dst);
    EXPECT_EQ(road->getDestinationIntersection(), src);
}


// ═════════════════════════════════════════════════════════════════════════════
//  7.  updateCongestion
// ═════════════════════════════════════════════════════════════════════════════

TEST_F(RoadTest, UpdateCongestion_ReturnsTrueOnValidInput)
{
    EXPECT_TRUE(road->updateCongestion(50));
}

TEST_F(RoadTest, UpdateCongestion_StoresNewLevel)
{
    road->updateCongestion(75);
    EXPECT_EQ(road->getCongestionLevel(), 75);
}

TEST_F(RoadTest, UpdateCongestion_RecalculatesTravelCost)
{
    road->updateCongestion(50);
    // 400 × 150 / 50 = 1200
    EXPECT_EQ(road->getTravelCost(), 1200);
}

TEST_F(RoadTest, UpdateCongestion_ReturnsFalseOnNegative)
{
    EXPECT_FALSE(road->updateCongestion(-1));
}

TEST_F(RoadTest, UpdateCongestion_ReturnsFalseAbove100)
{
    EXPECT_FALSE(road->updateCongestion(101));
}

TEST_F(RoadTest, UpdateCongestion_PreservesAllStateOnFailure)
{
    const int prevLevel = road->getCongestionLevel();
    const int prevCost  = road->getTravelCost();

    road->updateCongestion(-1);
    EXPECT_EQ(road->getCongestionLevel(), prevLevel);
    EXPECT_EQ(road->getTravelCost(),      prevCost);

    road->updateCongestion(101);
    EXPECT_EQ(road->getCongestionLevel(), prevLevel);
    EXPECT_EQ(road->getTravelCost(),      prevCost);
}

TEST_F(RoadTest, UpdateCongestion_BoundaryMinimum)
{
    ASSERT_TRUE(road->updateCongestion(Road::MIN_CONGESTION));
    EXPECT_EQ(road->getCongestionLevel(), Road::MIN_CONGESTION);
}

TEST_F(RoadTest, UpdateCongestion_BoundaryMaximum)
{
    ASSERT_TRUE(road->updateCongestion(Road::MAX_CONGESTION));
    EXPECT_EQ(road->getCongestionLevel(), Road::MAX_CONGESTION);
}

TEST_F(RoadTest, UpdateCongestion_BackToZeroRestoresBaseCost)
{
    road->updateCongestion(80);
    road->updateCongestion(0);
    EXPECT_EQ(road->getTravelCost(), DEFAULT_COST);
}

TEST_F(RoadTest, UpdateCongestion_IsIdempotent)
{
    road->updateCongestion(50);
    const int cost1 = road->getTravelCost();
    road->updateCongestion(50);
    EXPECT_EQ(road->getTravelCost(), cost1);
}


// ═════════════════════════════════════════════════════════════════════════════
//  8.  calculateTravelCost  –  formula, auto-update, edge-cases
//  I guess this is the most important part of the Road class, so it deserves a lot of tests to verify the formula 
//  is implemented correctly and behaves as expected across a wide range of inputs.
// ═════════════════════════════════════════════════════════════════════════════

TEST_F(RoadTest, TravelCost_FormulaWithNoCongestion)
{
    // 400 × 100 / 50 = 800
    EXPECT_EQ(road->getTravelCost(), 800);
}

TEST_F(RoadTest, TravelCost_MaxCongestionDoublesCost)
{
    const int base = road->getTravelCost();
    road->updateCongestion(100);
    // 400 × 200 / 50 = 1600 = 800 × 2
    EXPECT_EQ(road->getTravelCost(), base * 2);
}

TEST_F(RoadTest, TravelCost_IntermediateCongestion)
{
    road->updateCongestion(50);
    // 400 × 150 / 50 = 1200
    EXPECT_EQ(road->getTravelCost(), 1200);
}

TEST_F(RoadTest, TravelCost_HalvedSpeedDoublesCost)
{
    const int base = road->getTravelCost();  // 800
    road->setSpeedLimit(DEFAULT_SPEED / 2);  // 25 km/h
    // 400 × 100 / 25 = 1600
    EXPECT_EQ(road->getTravelCost(), base * 2);
}

TEST_F(RoadTest, TravelCost_IntegerDivisionTruncates_NotRounds)
{
    // 7 × 100 / 11 = 700 / 11 = 63  (bị truncate, không round lên 64)
    road->setDistance(7);
    road->setSpeedLimit(11);
    EXPECT_EQ(road->getTravelCost(), 700 / 11);  // 63
}

TEST_F(RoadTest, TravelCost_CongestionAndSpeedInteraction)
{
    road->updateCongestion(50);
    road->setSpeedLimit(100);
    // 400 × 150 / 100 = 600
    EXPECT_EQ(road->getTravelCost(), 600);
}

TEST_F(RoadTest, TravelCost_MultipleAttributeChangesCompound)
{
    road->setDistance(1000);
    road->setSpeedLimit(100);
    road->updateCongestion(50);
    // 1000 × 150 / 100 = 1500
    EXPECT_EQ(road->getTravelCost(), 1500);
}

TEST_F(RoadTest, TravelCost_AllFailedSettersLeaveEverythingUnchanged)
{
    const int dist       = road->getDistance();
    const int speed      = road->getSpeedLimit();
    const int congestion = road->getCongestionLevel();
    const int cost       = road->getTravelCost();

    road->setDistance(-1);
    road->setSpeedLimit(0);
    road->updateCongestion(999);
    road->setSourceIntersection(nullptr);
    road->setDestinationIntersection(nullptr);

    EXPECT_EQ(road->getDistance(),        dist);
    EXPECT_EQ(road->getSpeedLimit(),      speed);
    EXPECT_EQ(road->getCongestionLevel(), congestion);
    EXPECT_EQ(road->getTravelCost(),      cost);
    EXPECT_EQ(road->getSourceIntersection(),      src);
    EXPECT_EQ(road->getDestinationIntersection(), dst);
}

TEST(RoadTravelCost, ZeroWeightGuard_EnsuresMinimumOfOne)
{
    // distance=1, speed=130, congestion=0 → 1×100/130 = 0 → clamped to max(1, 0) = 1
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 0, 0, IntersectionType::CROSS);
    Road road("R", &src, &dst, Road::MIN_DISTANCE, Road::MAX_SPEED_LIMIT);

    EXPECT_GE(road.getTravelCost(), 1);
}

TEST(RoadTravelCost, IsAlwaysPositive_AcrossValidInputCombinations)
{
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 0, 0, IntersectionType::CROSS);

    for (int dist  : {1, 10, 100, Road::MAX_DISTANCE})
    for (int speed : {Road::MIN_SPEED_LIMIT, 50, Road::MAX_SPEED_LIMIT})
    {
        Road r("R", &src, &dst, dist, speed);
        EXPECT_GT(r.getTravelCost(), 0)
            << "distance=" << dist << " speed=" << speed;
    }
}

TEST(RoadTravelCost, MaxPossibleCostWithinIntRange)
{
    // MAX_DISTANCE + MIN_SPEED + MAX_CONGESTION → 8000×200/5 = 320 000
    Intersection src("A", 0, 0, IntersectionType::CROSS);
    Intersection dst("B", 0, 0, IntersectionType::CROSS);
    Road road("R", &src, &dst, Road::MAX_DISTANCE, Road::MIN_SPEED_LIMIT);
    road.updateCongestion(Road::MAX_CONGESTION);

    const int expected = (Road::MAX_DISTANCE * (100 + Road::MAX_CONGESTION))
                         / Road::MIN_SPEED_LIMIT;
    EXPECT_EQ(road.getTravelCost(), expected);
    EXPECT_GT(road.getTravelCost(), 0);  // không overflow thành âm
}

// ── Monotonicity ──────────────────────────────────────────────────────────────

TEST_F(RoadTest, TravelCost_IncreasesMonotonicallyWithCongestion)
{
    road->updateCongestion(0);
    int prev = road->getTravelCost();
    for (int c = 1; c <= Road::MAX_CONGESTION; ++c)
    {
        road->updateCongestion(c);
        EXPECT_GE(road->getTravelCost(), prev)
            << "Cost decreased when congestion increased to " << c;
        prev = road->getTravelCost();
    }
}

TEST_F(RoadTest, TravelCost_IncreasesMonotonicallyWithDistance)
{
    road->setDistance(Road::MIN_DISTANCE);
    int prev = road->getTravelCost();
    for (int d = 100; d <= 3000; d += 100)
    {
        road->setDistance(d);
        EXPECT_GE(road->getTravelCost(), prev)
            << "Cost decreased when distance increased to " << d;
        prev = road->getTravelCost();
    }
}

TEST_F(RoadTest, TravelCost_DecreasesMonotonicallyWithSpeed)
{
    road->setSpeedLimit(Road::MIN_SPEED_LIMIT);
    int prev = road->getTravelCost();
    for (int s = Road::MIN_SPEED_LIMIT + 5; s <= Road::MAX_SPEED_LIMIT; s += 5)
    {
        road->setSpeedLimit(s);
        EXPECT_LE(road->getTravelCost(), prev)
            << "Cost increased when speedLimit increased to " << s;
        prev = road->getTravelCost();
    }
}


int runRoadTests()
{
    // GTest needs at least argv[0] (program name); provide a dummy so that
    // InitGoogleTest() is satisfied when called from a host main() that may
    // not expose its own argc/argv.
    int   argc    = 1;
    char  name[]  = "Road_test";
    char* argv[]  = { name, nullptr };
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}