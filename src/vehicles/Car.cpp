#include "vehicles/Car.hpp"

namespace {
    constexpr double CAR_MAX_SPEED = 60.0; // map-units / s
    constexpr double CAR_LENGTH    = 5.0;  // map units
}

Car::Car(const std::string& id)
    : Vehicle(id, CAR_MAX_SPEED, CAR_LENGTH, VehicleType::CAR)
{
}
