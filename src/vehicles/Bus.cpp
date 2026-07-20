#include "vehicles/Bus.hpp"

namespace {
    constexpr double BUS_MAX_SPEED = 40.0; // map-units / s
    constexpr double BUS_LENGTH    = 12.0; // map units
}

Bus::Bus(const std::string& id)
    : Vehicle(id, BUS_MAX_SPEED, BUS_LENGTH, VehicleType::BUS)
{
}
