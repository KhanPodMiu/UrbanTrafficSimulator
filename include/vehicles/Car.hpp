#pragma once

#include "vehicles/Vehicle.hpp"

// Standard passenger car: the most common vehicle on the map, average speed
// and length. No special behavior beyond the base Vehicle.
class Car : public Vehicle {
public:
    explicit Car(const std::string& id);
};
