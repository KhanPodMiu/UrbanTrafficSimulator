#include "vehicles/EmergencyVehicle.hpp"

namespace {
    constexpr double EMERGENCY_MAX_SPEED = 300.0; // map-units / s
    constexpr double EMERGENCY_LENGTH    = 5.0;  // map units
}

EmergencyVehicle::EmergencyVehicle(const std::string& id)
    : Vehicle(id, EMERGENCY_MAX_SPEED, EMERGENCY_LENGTH, VehicleType::EMERGENCY)
{
}
