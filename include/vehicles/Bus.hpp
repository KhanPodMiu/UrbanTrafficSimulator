#pragma once

#include "vehicles/Vehicle.hpp"

// Public transit bus: longer than a car and a bit slower to accelerate/travel,
// which naturally makes it take up more room in the car-following model.
class Bus : public Vehicle {
public:
    explicit Bus(const std::string& id);
};
