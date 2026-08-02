#include "utils/event_handling.hpp"
#include "utils/InputHandler.hpp"

namespace {
    InputHandler g_inputHandler;
}

void handleInput(SDL_Event& event, AppContext &Game)
{
    g_inputHandler.handleInput(event, Game);
}

bool wasDragAction()
{
    return g_inputHandler.wasDragAction();
}

void spawnVehicleAt(const Vector2& clickPos, Graph& graph, VehicleManager& vehicleManager)
{
    g_inputHandler.spawnVehicleAt(clickPos, graph, vehicleManager);
}

bool switchBannedRoute(int routeID, AppContext &Game)
{
    return g_inputHandler.switchBannedRoute(routeID, Game);
}
