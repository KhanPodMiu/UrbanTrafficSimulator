#pragma once
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

class TestGraphBuilder
{
public:
    static Graph createGrid(
        int rows,
        int cols,
        int distance = 100,
        int speedLimit = 60,
        bool twoWay = true
    );
};