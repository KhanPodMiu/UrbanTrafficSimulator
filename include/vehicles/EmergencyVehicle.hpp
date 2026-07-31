#pragma once

#include "vehicles/Vehicle.hpp"

// Ambulance / fire-truck / police style vehicle: faster than normal traffic
// and allowed to run red lights (still yields to physical car-following so it
// doesn't rear-end whoever is stopped in front of it).
class EmergencyVehicle : public Vehicle {
public:
    explicit EmergencyVehicle(const std::string& id);

    bool ignoresTrafficLights() const override { return true; }
};
