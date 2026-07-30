#pragma once

#include "vehicles/Vehicle.hpp"

// Ambulance / fire-truck / police style vehicle: faster than normal traffic,
// allowed to run red lights, and allowed to pull into the shoulder lane to
// get around slower/blocked traffic instead of slowing down behind it.
class EmergencyVehicle : public Vehicle {
public:
    explicit EmergencyVehicle(const std::string& id);

    bool ignoresTrafficLights() const override { return true; }

    // Instead of slowing down behind a vehicle that's blocking its lane, an
    // emergency vehicle pulls out to the right-hand shoulder and keeps going.
    bool canOvertakeBlockedTraffic() const override { return true; }
};