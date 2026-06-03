// ─────────────────────────────────────────────────────────────────────────────
//  Road_test.cpp
//  Stand-alone unit tests for the Road class.
//  No external test framework required – compile and run independently of the
//  main SDL2 application.
//
//  Build example (from project root):
//    g++ -std=c++17 -I. src/graph/Road.cpp src/graph/Intersection.cpp
//        tests/Road_test.cpp -o Road_test && ./Road_test
// ─────────────────────────────────────────────────────────────────────────────

#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <iostream>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  Minimal test runner
// ─────────────────────────────────────────────────────────────────────────────
namespace Test
{
    static int s_pass  = 0;
    static int s_fail  = 0;
    static int s_total = 0;

    void check(bool result, const char* expression, int line)
    {
        ++s_total;
        if (result)
        {
            ++s_pass;
        }
        else
        {
            ++s_fail;
            std::cerr << "  [FAIL] line " << line << ":  " << expression << "\n";
        }
    }

    void beginGroup(const char* name)
    {
        std::cout << "\n[" << name << "]\n";
    }

    void printSummary()
    {
        std::cout << "\n════════════════════════════════════════\n"
                  << "  Road unit tests\n"
                  << "  Total : " << s_total << "\n"
                  << "  Passed: " << s_pass  << "\n"
                  << "  Failed: " << s_fail  << "\n"
                  << "════════════════════════════════════════\n";
    }

    int exitCode() { return (s_fail == 0) ? 0 : 1; }
}

// Convenience macro – captures expression text and line number automatically.
#define CHECK(expr) Test::check((expr), #expr, __LINE__)

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────────────────────
static Intersection makeIntersection(const std::string& id, int x, int y)
{
    return Intersection(id, x, y, IntersectionType::CROSS);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: Construction with valid arguments
//  Verifies that the constructor stores every attribute correctly and that
//  travelCost is computed immediately (not left as 0).
// ─────────────────────────────────────────────────────────────────────────────
void test_construction_valid()
{
    Test::beginGroup("Construction – valid arguments");

    Intersection src = makeIntersection("A", 100, 100);
    Intersection dst = makeIntersection("B", 500, 100);

    Road road("R1", &src, &dst, 400, 50);

    CHECK(road.getRoadId()                  == "R1");
    CHECK(road.getSourceIntersection()      == &src);
    CHECK(road.getDestinationIntersection() == &dst);
    CHECK(road.getDistance()                == 400);
    CHECK(road.getSpeedLimit()              == 50);
    CHECK(road.getCongestionLevel()         == 0);

    // travelCost = max(1,  400 × (100 + 0) / 50 )
    //           = max(1, 40 000 / 50)
    //           = max(1, 800) = 800
    CHECK(road.getTravelCost() == 800);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: Constructor clamps out-of-range distance / speedLimit
//  Objects must always be in a valid state after construction.
// ─────────────────────────────────────────────────────────────────────────────
void test_construction_clampsInvalidValues()
{
    Test::beginGroup("Construction – clamps out-of-range values");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 10, 10);

    // distance = -500 → clamped to MIN_DISTANCE = 1
    // speedLimit = 999 → clamped to MAX_SPEED_LIMIT = 130
    Road road("R2", &src, &dst, -500, 999);

    CHECK(road.getDistance()   == Road::MIN_DISTANCE);
    CHECK(road.getSpeedLimit() == Road::MAX_SPEED_LIMIT);

    // travelCost = max(1,  1 × (100 + 0) / 130 )
    //           = max(1, 0) = 1
    CHECK(road.getTravelCost() == 1);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: setDistance
// ─────────────────────────────────────────────────────────────────────────────
void test_setDistance()
{
    Test::beginGroup("setDistance");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 0, 0);
    Road road("R", &src, &dst, 500, 50);

    // ── valid change ─────────────────────────────────────────────────────────
    CHECK(road.setDistance(1000) == true);
    CHECK(road.getDistance()     == 1000);
    // travelCost must be recalculated: 1000 × 100 / 50 = 2000
    CHECK(road.getTravelCost()   == 2000);

    // ── invalid: zero ────────────────────────────────────────────────────────
    CHECK(road.setDistance(0)    == false);
    CHECK(road.getDistance()     == 1000); // unchanged

    // ── invalid: negative ────────────────────────────────────────────────────
    CHECK(road.setDistance(-1)   == false);
    CHECK(road.getDistance()     == 1000);

    // ── invalid: above maximum ───────────────────────────────────────────────
    CHECK(road.setDistance(Road::MAX_DISTANCE + 1) == false);
    CHECK(road.getDistance()                       == 1000);

    // ── boundary: exact minimum ──────────────────────────────────────────────
    CHECK(road.setDistance(Road::MIN_DISTANCE) == true);
    CHECK(road.getDistance()                   == Road::MIN_DISTANCE);

    // ── boundary: exact maximum ──────────────────────────────────────────────
    CHECK(road.setDistance(Road::MAX_DISTANCE) == true);
    CHECK(road.getDistance()                   == Road::MAX_DISTANCE);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: setSpeedLimit
// ─────────────────────────────────────────────────────────────────────────────
void test_setSpeedLimit()
{
    Test::beginGroup("setSpeedLimit");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 0, 0);
    Road road("R", &src, &dst, 500, 50);

    // ── valid change ─────────────────────────────────────────────────────────
    CHECK(road.setSpeedLimit(100) == true);
    CHECK(road.getSpeedLimit()    == 100);
    // travelCost: 500 × 100 / 100 = 500
    CHECK(road.getTravelCost()    == 500);

    // ── invalid: below minimum ───────────────────────────────────────────────
    CHECK(road.setSpeedLimit(Road::MIN_SPEED_LIMIT - 1) == false);
    CHECK(road.getSpeedLimit()                          == 100); // unchanged

    // ── invalid: zero ────────────────────────────────────────────────────────
    CHECK(road.setSpeedLimit(0) == false);
    CHECK(road.getSpeedLimit()  == 100);

    // ── invalid: negative ────────────────────────────────────────────────────
    CHECK(road.setSpeedLimit(-10) == false);
    CHECK(road.getSpeedLimit()    == 100);

    // ── invalid: above maximum ───────────────────────────────────────────────
    CHECK(road.setSpeedLimit(Road::MAX_SPEED_LIMIT + 1) == false);
    CHECK(road.getSpeedLimit()                          == 100);

    // ── boundary: exact minimum ──────────────────────────────────────────────
    CHECK(road.setSpeedLimit(Road::MIN_SPEED_LIMIT) == true);
    CHECK(road.getSpeedLimit()                      == Road::MIN_SPEED_LIMIT);

    // ── boundary: exact maximum ──────────────────────────────────────────────
    CHECK(road.setSpeedLimit(Road::MAX_SPEED_LIMIT) == true);
    CHECK(road.getSpeedLimit()                      == Road::MAX_SPEED_LIMIT);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: setSourceIntersection
// ─────────────────────────────────────────────────────────────────────────────
void test_setSourceIntersection()
{
    Test::beginGroup("setSourceIntersection");

    Intersection a = makeIntersection("A", 0, 0);
    Intersection b = makeIntersection("B", 10, 10);
    Intersection c = makeIntersection("C", 20, 20);
    Road road("R", &a, &b, 100, 50);

    // ── nullptr rejected ─────────────────────────────────────────────────────
    CHECK(road.setSourceIntersection(nullptr) == false);
    CHECK(road.getSourceIntersection()        == &a);  // unchanged

    // ── valid pointer accepted ───────────────────────────────────────────────
    CHECK(road.setSourceIntersection(&c) == true);
    CHECK(road.getSourceIntersection()   == &c);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: setDestinationIntersection
// ─────────────────────────────────────────────────────────────────────────────
void test_setDestinationIntersection()
{
    Test::beginGroup("setDestinationIntersection");

    Intersection a = makeIntersection("A", 0, 0);
    Intersection b = makeIntersection("B", 10, 10);
    Intersection c = makeIntersection("C", 20, 20);
    Road road("R", &a, &b, 100, 50);

    // ── nullptr rejected ─────────────────────────────────────────────────────
    CHECK(road.setDestinationIntersection(nullptr) == false);
    CHECK(road.getDestinationIntersection()        == &b);  // unchanged

    // ── valid pointer accepted ───────────────────────────────────────────────
    CHECK(road.setDestinationIntersection(&c) == true);
    CHECK(road.getDestinationIntersection()   == &c);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: updateCongestion
//  Verifies range validation AND that travelCost is recalculated each time.
// ─────────────────────────────────────────────────────────────────────────────
void test_updateCongestion()
{
    Test::beginGroup("updateCongestion");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 0, 0);

    // distance=400, speedLimit=50, congestion=0  →  400×100/50 = 800
    Road road("R", &src, &dst, 400, 50);

    CHECK(road.getCongestionLevel() == 0);
    CHECK(road.getTravelCost()      == 800);

    // ── congestion = 50: 400 × 150 / 50 = 1 200 ─────────────────────────────
    CHECK(road.updateCongestion(50)  == true);
    CHECK(road.getCongestionLevel()  == 50);
    CHECK(road.getTravelCost()       == 1200);

    // ── congestion = 100 (max): 400 × 200 / 50 = 1 600 ──────────────────────
    CHECK(road.updateCongestion(100) == true);
    CHECK(road.getCongestionLevel()  == 100);
    CHECK(road.getTravelCost()       == 1600);

    // ── congestion back to 0: cost returns to 800 ────────────────────────────
    CHECK(road.updateCongestion(0)   == true);
    CHECK(road.getCongestionLevel()  == 0);
    CHECK(road.getTravelCost()       == 800);

    // ── invalid: negative ────────────────────────────────────────────────────
    CHECK(road.updateCongestion(-1)  == false);
    CHECK(road.getCongestionLevel()  == 0);   // unchanged
    CHECK(road.getTravelCost()       == 800); // cost unchanged

    // ── invalid: above 100 ───────────────────────────────────────────────────
    CHECK(road.updateCongestion(101) == false);
    CHECK(road.getCongestionLevel()  == 0);
    CHECK(road.getTravelCost()       == 800);

    // ── boundary: exact minimum ──────────────────────────────────────────────
    CHECK(road.updateCongestion(Road::MIN_CONGESTION) == true);

    // ── boundary: exact maximum ──────────────────────────────────────────────
    CHECK(road.updateCongestion(Road::MAX_CONGESTION) == true);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: calculateTravelCost formula
//  Uses concrete numbers to pin the exact formula so any future change
//  in the formula is caught immediately.
// ─────────────────────────────────────────────────────────────────────────────
void test_calculateTravelCost_formula()
{
    Test::beginGroup("calculateTravelCost – formula verification");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 0, 0);

    // ── Case 1: distance=1000, speed=100, congestion=0 ───────────────────────
    //    cost = max(1, 1000 × 100 / 100) = 1000
    {
        Road r("R", &src, &dst, 1000, 100);
        CHECK(r.getTravelCost() == 1000);
    }

    // ── Case 2: halve the speed → cost doubles ───────────────────────────────
    //    distance=1000, speed=50, congestion=0 → 1000×100/50 = 2000
    {
        Road r("R", &src, &dst, 1000, 50);
        CHECK(r.getTravelCost() == 2000);
    }

    // ── Case 3: max congestion doubles cost (the core acceptance criterion) ──
    //    distance=1000, speed=100, congestion=100 → 1000×200/100 = 2000
    {
        Road r("R", &src, &dst, 1000, 100);
        int costFree = r.getTravelCost();        // 1000

        r.updateCongestion(100);
        int costMax  = r.getTravelCost();        // 2000

        CHECK(costFree != 0);
        CHECK(costMax  == costFree * 2);
    }

    // ── Case 4: intermediate congestion ──────────────────────────────────────
    //    distance=1000, speed=100, congestion=50 → 1000×150/100 = 1500
    {
        Road r("R", &src, &dst, 1000, 100);
        r.updateCongestion(50);
        CHECK(r.getTravelCost() == 1500);
    }

    // ── Case 5: zero-weight guard ─────────────────────────────────────────────
    //    Very short road at max speed: distance=1, speed=130, congestion=0
    //    raw = 1×100/130 = 0  →  clamped to max(1, 0) = 1
    {
        Road r("R", &src, &dst, 1, 130);
        CHECK(r.getTravelCost() >= 1);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: travelCost auto-updates when setters change relevant attributes
// ─────────────────────────────────────────────────────────────────────────────
void test_calculateTravelCost_autoUpdates()
{
    Test::beginGroup("calculateTravelCost – auto-updates via setters");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 0, 0);

    // Initial: distance=500, speed=50, congestion=0 → 500×100/50 = 1000
    Road road("R", &src, &dst, 500, 50);
    CHECK(road.getTravelCost() == 1000);

    // Double the distance: 1000×100/50 = 2000
    road.setDistance(1000);
    CHECK(road.getTravelCost() == 2000);

    // Double the speed: 1000×100/100 = 1000
    road.setSpeedLimit(100);
    CHECK(road.getTravelCost() == 1000);

    // Add 50% congestion: 1000×150/100 = 1500
    road.updateCongestion(50);
    CHECK(road.getTravelCost() == 1500);

    // Failed setter must NOT change travelCost
    road.setDistance(-99);  // rejected
    CHECK(road.getTravelCost() == 1500); // unchanged

    road.setSpeedLimit(0);  // rejected
    CHECK(road.getTravelCost() == 1500);

    road.updateCongestion(999); // rejected
    CHECK(road.getTravelCost() == 1500);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test: monotonicity of travelCost
//  More congestion / longer distance → higher cost; higher speed → lower cost.
// ─────────────────────────────────────────────────────────────────────────────
void test_travelCost_monotonicity()
{
    Test::beginGroup("travelCost – monotonicity");

    Intersection src = makeIntersection("A", 0, 0);
    Intersection dst = makeIntersection("B", 0, 0);

    // Congestion: cost increases as congestion increases
    {
        Road r("R", &src, &dst, 1000, 60);
        int prev = r.getTravelCost();
        for (int c = 10; c <= 100; c += 10)
        {
            r.updateCongestion(c);
            CHECK(r.getTravelCost() >= prev);
            prev = r.getTravelCost();
        }
    }

    // Distance: cost increases as distance increases
    {
        Road r("R", &src, &dst, Road::MIN_DISTANCE, 60);
        int prev = r.getTravelCost();
        for (int d = 100; d <= 2000; d += 100)
        {
            r.setDistance(d);
            CHECK(r.getTravelCost() >= prev);
            prev = r.getTravelCost();
        }
    }

    // Speed: cost decreases as speed limit increases
    {
        Road r("R", &src, &dst, 1000, Road::MIN_SPEED_LIMIT);
        int prev = r.getTravelCost();
        for (int s = Road::MIN_SPEED_LIMIT + 5; s <= Road::MAX_SPEED_LIMIT; s += 5)
        {
            r.setSpeedLimit(s);
            CHECK(r.getTravelCost() <= prev);
            prev = r.getTravelCost();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    std::cout << "Running Road unit tests…\n";

    test_construction_valid();
    test_construction_clampsInvalidValues();
    test_setDistance();
    test_setSpeedLimit();
    test_setSourceIntersection();
    test_setDestinationIntersection();
    test_updateCongestion();
    test_calculateTravelCost_formula();
    test_calculateTravelCost_autoUpdates();
    test_travelCost_monotonicity();

    Test::printSummary();
    return Test::exitCode();
}