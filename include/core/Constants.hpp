#pragma once

// Single source of truth for layout / window / map / camera / render constants.
// Domain-specific constants that belong to a class's own business rules
// (e.g. Road's speed-limit / congestion / signal-duration bounds) are kept
// inside that class's header instead of here, since they are not related
// to visualization or window layout.
namespace Config
{
    // ---- Window layout -----------------------------------------------
    constexpr int WINDOW_WIDTH  = 1600;
    constexpr int WINDOW_HEIGHT = 900;
    constexpr int PANEL_WIDTH   = 400;

    // Camera viewport = window area minus the left UI panel.
    // Derived instead of duplicated so it can never drift from WINDOW_WIDTH/PANEL_WIDTH.
    // (1600 - 400 = 1200, same numeric value as the original hard-coded constant.)
    constexpr int VIEW_PORT_WIDTH  = WINDOW_WIDTH - PANEL_WIDTH;
    constexpr int VIEW_PORT_HEIGHT = WINDOW_HEIGHT;

    // ---- Map (world) dimensions ---------------------------------------
    constexpr int MAP_WIDTH  = 53000;
    constexpr int MAP_HEIGHT = 40000;

    // ---- Camera behavior ------------------------------------------------
    constexpr float MOVE_SPEED = 40.0f;

    // NOTE: preserved exactly as in the original main.cpp. WINDOW_WIDTH and
    // MAP_WIDTH are both int, so this is integer division and always
    // evaluates to 0.0f before being clamped up to MINZOOM inside
    // Camera::setZoom(). This looks like a pre-existing bug, but per
    // instructions this refactor does not change any logic/behavior.
    constexpr float INITIAL_CAMERA_SCALE = WINDOW_WIDTH / MAP_WIDTH;

    // ---- Rendering ------------------------------------------------------
    constexpr int ROUNDABOUT_RADIUS = 150;
    constexpr int ROAD_WIDTH        = 80;

    // ---- Simulation timing ------------------------------------------------
    constexpr double TARGET_FPS        = 30.0;
    constexpr double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    constexpr int TRAFFIC_LIGHT_MARKER_SIZE = 40;
}
