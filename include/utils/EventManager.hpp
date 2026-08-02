#pragma once

struct AppContext;
class Camera;
class EntityInfoPanel;
class StatisticsPanel;
class VehicleManager;
class VisualizationEngine;
class WorldClock;

class EventManager {
public:
    static void processEvents(
        AppContext& context,
        WorldClock& clock,
        StatisticsPanel& statisticsPanel,
        EntityInfoPanel& entityInfoPanel,
        VehicleManager& vehicleManager,
        Camera& camera,
        VisualizationEngine& visualizationEngine);
};
