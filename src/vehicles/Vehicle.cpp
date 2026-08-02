#include "vehicles/Vehicle.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "graph/Intersection.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "algorithms/RoutingManager.hpp"
// #include "core/Constants.hpp" // Legacy roundabout-arc implementation only.
#include "simulation/RouteRequest.hpp"

namespace {
    constexpr double COMFORT_ACCEL = 35.0; 
    constexpr double LANE_OFFSET = 40.0; 
    // Extra lateral offset (added on top of LANE_OFFSET) applied while a
    // vehicle has pulled out into the shoulder to get around blocked traffic.
    constexpr double PASSING_LANE_OFFSET = 40.0;
}

Vehicle::Vehicle(const std::string& id, double maxSpeed, double length, VehicleType type)
    : m_id(id),
      m_type(type),
      m_currentSpeed(0.0),
      m_targetSpeed(maxSpeed),
      m_maxSpeed(maxSpeed),
      m_distanceOnRoad(0.0),
      m_length(length),
      m_finished(false),
      m_currentRoad(nullptr),
      m_destination(nullptr),
      m_routeOptimizer(nullptr),
      m_routeIndex(0),
      m_position{0.0, 0.0}
{
}

Vehicle::Vehicle(const std::string& id, double maxSpeed)
    : Vehicle(id, maxSpeed, 5.0, VehicleType::CAR)
{
}

void Vehicle::update(double dt)
{
    if (m_finished)
        return;

    /*
    // Legacy behavior (disabled): stopping immediately could strand a vehicle
    // in the middle of a road at the exact moment that road became restricted.
    if (m_currentRoad && m_currentRoad->isVIPExclusive())
    {
        m_currentSpeed = 0.0;
        m_targetSpeed = 0.0;
        return;
    }
    */

    // A vehicle already on a newly restricted road is allowed to keep moving
    // forward until it reaches that road's destination intersection. Its
    // remaining route is replaced by rerouteAroundBannedRoads(), so it can
    // leave the restricted segment physically instead of being teleported or
    // frozen inside it.

    if (m_currentSpeed < m_targetSpeed)
    {
        m_currentSpeed = std::min(m_targetSpeed, m_currentSpeed + COMFORT_ACCEL * dt);
    }
    else if (m_currentSpeed > m_targetSpeed)
    {
        m_currentSpeed = std::max(m_targetSpeed, m_currentSpeed - COMFORT_ACCEL * dt);
    }

    if (m_currentSpeed < 0.0)
        m_currentSpeed = 0.0;

    m_distanceOnRoad += m_currentSpeed * dt;

    updateWorldPosition();
}

void Vehicle::assignRoute(const std::shared_ptr<RouteOptimizer>& optimizer)
{
    m_routeOptimizer = optimizer;
}

void Vehicle::updateWorldPosition()
{
    if (!m_currentRoad)
        return;

    const Intersection* src = m_currentRoad->getSourceIntersection();
    const Intersection* dst = m_currentRoad->getDestinationIntersection();

    if (!src || !dst)
        return;

    double roadLength = m_currentRoad->getDistance();
    double dist = m_distanceOnRoad;

    /*
     * Legacy roundabout-arc implementation (disabled).
     * Kept here for reference in case curved movement is needed again later.
     *
    auto calculateRoundaboutArc = [&](const Intersection* r_center, const Intersection* in_src, const Intersection* out_dst, double t_arc) {
        double R = Config::ROUNDABOUT_RADIUS;
        
        double dx0 = r_center->getX() - in_src->getX();
        double dy0 = r_center->getY() - in_src->getY();
        double len0 = std::sqrt(dx0*dx0 + dy0*dy0);
        
        double dx1 = out_dst->getX() - r_center->getX();
        double dy1 = out_dst->getY() - r_center->getY();
        double len1 = std::sqrt(dx1*dx1 + dy1*dy1);
        
        if (len0 < 1e-5 || len1 < 1e-5) return false;
        
        double p0_x = r_center->getX() - (dx0/len0)*R - (dy0/len0)*LANE_OFFSET;
        double p0_y = r_center->getY() - (dy0/len0)*R + (dx0/len0)*LANE_OFFSET;
        
        double p3_x = r_center->getX() + (dx1/len1)*R - (dy1/len1)*LANE_OFFSET;
        double p3_y = r_center->getY() + (dy1/len1)*R + (dx1/len1)*LANE_OFFSET;
        
        double theta_start = std::atan2(p0_y - r_center->getY(), p0_x - r_center->getX());
        double theta_end = std::atan2(p3_y - r_center->getY(), p3_x - r_center->getX());
        
        if (theta_end >= theta_start) {
            theta_end -= 2.0 * M_PI;
        }
        
        double theta = theta_start + (theta_end - theta_start) * t_arc;
        double radius = std::sqrt(R*R + LANE_OFFSET*LANE_OFFSET);
        
        m_position = Vector2(static_cast<float>(r_center->getX() + radius * std::cos(theta)),
                             static_cast<float>(r_center->getY() + radius * std::sin(theta)));
        return true;
    };

    double R = Config::ROUNDABOUT_RADIUS;
    
    // Check if we are approaching a roundabout
    if (dst->getType() == IntersectionType::ROUNDABOUT && (roadLength - dist) < R)
    {
        std::shared_ptr<Road> nextRoad = getNextRoad();
        if (nextRoad)
        {
            const Intersection* next_dst = nextRoad->getDestinationIntersection();
            if (next_dst)
            {
                double d_arc = R - (roadLength - dist);
                double t_arc = std::clamp(d_arc / (2.0 * R), 0.0, 0.5);
                if (calculateRoundaboutArc(dst, src, next_dst, t_arc))
                    return;
            }
        }
    }
    // Check if we are leaving a roundabout
    else if (src->getType() == IntersectionType::ROUNDABOUT && dist < R)
    {
        if (m_routeIndex > 0)
        {
            std::shared_ptr<Road> prevRoad = m_route[m_routeIndex - 1];
            const Intersection* prev_src = prevRoad->getSourceIntersection();
            if (prev_src)
            {
                double d_arc = R + dist;
                double t_arc = std::clamp(d_arc / (2.0 * R), 0.5, 1.0);
                if (calculateRoundaboutArc(src, prev_src, dst, t_arc))
                    return;
            }
        }
    }
    */

    // Roundabouts intentionally use the same straight road interpolation as
    // every other intersection. The active road changes only in
    // tryAdvanceToNextRoad(), so the vehicle heads directly into the junction
    // and continues along the next road without a synthetic circular arc.
    double t = (roadLength > 0.0) ? (dist / roadLength) : 0.0;
    t = std::clamp(t, 0.0, 1.0);

    double x0 = src->getX();
    double y0 = src->getY();
    double x1 = dst->getX();
    double y1 = dst->getY();

    double dx = x1 - x0;
    double dy = y1 - y0;

    double baseX = x0 + dx * t;
    double baseY = y0 + dy * t;

    if (roadLength > 0.0)
    {
        // Perpendicular unit vector (right-hand side of travel direction).
        double nx = -dy / roadLength;
        double ny = dx / roadLength;

        double offset = LANE_OFFSET + (m_isPassing ? PASSING_LANE_OFFSET : 0.0);

        baseX += nx * offset;
        baseY += ny * offset;
    }

    m_position = Vector2(static_cast<float>(baseX), static_cast<float>(baseY));
}

bool Vehicle::tryAdvanceToNextRoad()
{
    if (!m_currentRoad)
        return false;

    if (m_distanceOnRoad < m_currentRoad->getDistance())
        return false; 

    if (!m_currentRoad->isGreen() && !ignoresTrafficLights() && !m_committedToIntersection)
    {
        m_distanceOnRoad = m_currentRoad->getDistance();
        m_currentSpeed = 0.0;
        updateWorldPosition();
        return false;
    }

    double overflow = m_distanceOnRoad - m_currentRoad->getDistance();

    std::shared_ptr<Road> next = getNextRoad();

    // Never enter a restricted road from an unrestricted one. Normally the
    // ban event has already installed a detour; this guard safely handles the
    // case where no valid detour exists.
    // if (next && next->isVIPExclusive()) // Legacy: active-only closure.
    if (next && next->isUnavailableForRouting())
    {
        m_distanceOnRoad = m_currentRoad->getDistance();
        m_currentSpeed = 0.0;
        m_targetSpeed = 0.0;
        updateWorldPosition();
        return false;
    }

    m_currentRoad->vehicleExits(this);

    if (!next)
    {
        // No more roads in the route: trip complete.
        m_finished = true;
        m_distanceOnRoad = m_currentRoad->getDistance();
        updateWorldPosition();
        return true;
    }

    m_currentRoad = next;
    m_routeIndex++;
    m_distanceOnRoad = overflow;

    m_currentRoad->vehicleEnters(this);

    updateWorldPosition();
    m_committedToIntersection = false;
    m_isPassing = false; // fresh road, fresh lane
    return true;
}

bool Vehicle::rerouteAroundBannedRoads(
    const Graph& graph,
    const RoutingManager& routingManager)
{
    if (m_finished || !m_currentRoad || !m_destination)
        return false;

    // const bool currentRoadIsBanned =
    //     m_currentRoad->isUnavailableForRouting(); // Legacy teleport branch.
    bool remainingRouteContainsBannedRoad = false;
    for (size_t i = m_routeIndex; i < m_route.size(); ++i)
    {
        // if (m_route[i] && m_route[i]->isVIPExclusive()) // Legacy.
        if (m_route[i] && m_route[i]->isUnavailableForRouting())
        {
            remainingRouteContainsBannedRoad = true;
            break;
        }
    }

    if (!remainingRouteContainsBannedRoad)
        return false;

    // A vehicle can only leave its current directed road through the forward
    // endpoint. The active router (Dijkstra in the simulation) then minimizes
    // congestion-aware travel cost from that exit to the destination.
    const Intersection* rerouteStart =
        m_currentRoad->getDestinationIntersection();
    if (!rerouteStart)
        return false;

    const std::string destinationID =
        m_destination->getIntersectionID();
    if (rerouteStart->getIntersectionID() == destinationID)
    {
        m_route = {m_currentRoad};
        m_routeIndex = 0;
        return true;
    }

    const RouteResult routeResult = routingManager.calculateRoute(
        graph,
        RouteRequest(rerouteStart->getIntersectionID(), destinationID));
    if (!routeResult.isSuccess || routeResult.intersectionIDs.size() < 2)
    {
        /*
        // Legacy behavior (disabled): a vehicle outside the corridor kept its
        // old route when no detour existed, so that route still pointed into
        // a road scheduled for closure.
        if (!currentRoadIsBanned)
            return false;
        */

        // No legal detour exists. End the plan at the current safe road so no
        // remaining route can point into the presidential corridor. A vehicle
        // already inside the corridor still reaches its forward endpoint.
        m_route = {m_currentRoad};
        m_routeIndex = 0;
        return true;
    }

    std::vector<std::shared_ptr<Road>> replacementRoute;
    replacementRoute.reserve(routeResult.intersectionIDs.size());
    replacementRoute.push_back(m_currentRoad);

    for (size_t i = 0; i + 1 < routeResult.intersectionIDs.size(); ++i)
    {
        Road* road = graph.getRoadBetween(
            routeResult.intersectionIDs[i],
            routeResult.intersectionIDs[i + 1]);
        // if (!road || road->isVIPExclusive()) // Legacy: active-only closure.
        if (!road || road->isUnavailableForRouting())
            return false;

        std::shared_ptr<Road> roadPtr = graph.getRoad(road->getRoadId());
        if (!roadPtr)
            return false;

        replacementRoute.push_back(roadPtr);
    }

    if (replacementRoute.empty())
        return false;

    /*
    // Legacy behavior (disabled): this replaced the current road immediately
    // and copied its percentage progress, which visually teleported the car.
    const double currentRoadProgress = m_currentRoad->getDistance() > 0.0
        ? std::clamp(
              m_distanceOnRoad / m_currentRoad->getDistance(),
              0.0,
              1.0)
        : 0.0;
    if (currentRoadIsBanned)
    {
        m_currentRoad->vehicleExits(this);
        m_currentRoad = replacementRoute.front();
        m_distanceOnRoad =
            currentRoadProgress * m_currentRoad->getDistance();
        m_committedToIntersection = false;
        m_isPassing = false;
        m_currentRoad->vehicleEnters(this);
    }
    */

    m_route = std::move(replacementRoute);
    m_routeIndex = 0;
    updateWorldPosition();
    return true;
}

const std::string& Vehicle::getId() const
{
    return m_id;
}

VehicleType Vehicle::getType() const
{
    return m_type;
}

std::shared_ptr<Road> Vehicle::getCurrentRoad() const
{
    return m_currentRoad;
}

std::shared_ptr<Intersection> Vehicle::getDestination() const
{
    return m_destination;
}

Vector2 Vehicle::getPosition() const
{
    return m_position;
}

double Vehicle::getCurrentSpeed() const
{
    return m_currentSpeed;
}

double Vehicle::getTargetSpeed() const
{
    return m_targetSpeed;
}

double Vehicle::getMaxSpeed() const
{
    return m_maxSpeed;
}

double Vehicle::getDistanceOnRoad() const
{
    return m_distanceOnRoad;
}

double Vehicle::getVehicleLength() const
{
    return m_length;
}

bool Vehicle::isFinished() const
{
    return m_finished;
}

double Vehicle::getMovementProgress() const
{
    if (!m_currentRoad)
        return 0.0;

    return m_distanceOnRoad / m_currentRoad->getDistance();
}

const std::vector<std::shared_ptr<Road>>& Vehicle::getRoute() const
{
    return m_route;
}

size_t Vehicle::getRouteIndex() const
{
    return m_routeIndex;
}

std::string Vehicle::getSourceIntersectionId() const
{
    if (m_route.empty() || !m_route.front())
        return "";

    const Intersection* source = m_route.front()->getSourceIntersection();
    return source != nullptr ? source->getIntersectionID() : "";
}

std::string Vehicle::getDestinationIntersectionId() const
{
    return m_destination != nullptr ? m_destination->getIntersectionID() : "";
}

std::string Vehicle::getCurrentRoadId() const
{
    return m_currentRoad != nullptr ? m_currentRoad->getRoadId() : "";
}

void Vehicle::setCurrentRoad(const std::shared_ptr<Road>& road)
{
    m_currentRoad = road;
}

void Vehicle::setDestination(const std::shared_ptr<Intersection>& destination)
{
    m_destination = destination;
}

void Vehicle::setPosition(const Vector2& position)
{
    m_position = position;
}

void Vehicle::setCurrentSpeed(double speed)
{
    m_currentSpeed = speed;
}

void Vehicle::setTargetSpeed(double speed)
{
    m_targetSpeed = speed;
}

bool Vehicle::isInPassingLane() const
{
    return m_isPassing;
}

void Vehicle::setInPassingLane(bool passing)
{
    // Sticky: once a vehicle has pulled out to pass, it stays out until it
    // moves onto a different road. We only ever flip it true here; nothing
    // resets it back to false while still on the same road.
    if (passing)
        m_isPassing = true;
}

bool Vehicle::isCommittedToIntersection() const
{
    return m_committedToIntersection;
}

void Vehicle::setCommittedToIntersection(bool committed)
{
    m_committedToIntersection = committed;
}

void Vehicle::setDistanceOnRoad(double distance)
{
    m_distanceOnRoad = distance;
}

void Vehicle::setRoute(const std::vector<std::shared_ptr<Road>>& route)
{
    m_route = route;
    m_routeIndex = 0;
}

void Vehicle::setRouteIndex(size_t index)
{
    m_routeIndex = index;
}

void Vehicle::setFinished(bool finished)
{
    m_finished = finished;
}

void Vehicle::advanceDistance(double distance)
{
    m_distanceOnRoad += distance;
}

std::shared_ptr<Road> Vehicle::getNextRoad() const
{
    if (m_routeIndex + 1 >= m_route.size())
        return nullptr;

    return m_route[m_routeIndex + 1];
}

double Vehicle::getHeadingAngle() const
{
    if (!m_currentRoad)
        return 0.0;

    const Intersection* src = m_currentRoad->getSourceIntersection();
    const Intersection* dst = m_currentRoad->getDestinationIntersection();

    double dx = dst->getX() - src->getX();
    double dy = dst->getY() - src->getY();

    return atan2(dy, dx) * 180.0 / M_PI;
}
