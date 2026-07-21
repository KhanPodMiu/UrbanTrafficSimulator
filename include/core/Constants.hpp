#pragma once

namespace Config
{
    // ---- Window layout -----------------------------------------------
    constexpr int WINDOW_WIDTH  = 1600;
    constexpr int WINDOW_HEIGHT = 900;
    constexpr int PANEL_WIDTH   = 400;

    constexpr int VIEW_PORT_WIDTH  = WINDOW_WIDTH - PANEL_WIDTH;
    constexpr int VIEW_PORT_HEIGHT = WINDOW_HEIGHT;

    // ---- Map (world) dimensions ---------------------------------------
    constexpr int MAP_WIDTH  = 53000;
    constexpr int MAP_HEIGHT = 40000;

    // ---- Camera behavior ------------------------------------------------
    constexpr float MOVE_SPEED = 40.0f;

    constexpr float INITIAL_CAMERA_SCALE = WINDOW_WIDTH / MAP_WIDTH;

    // ---- Rendering ------------------------------------------------------
    constexpr int ROUNDABOUT_RADIUS = 80;
    constexpr int ROAD_WIDTH        = 80;

    // ---- Simulation timing ------------------------------------------------
    constexpr double TARGET_FPS        = 30.0;
    constexpr double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    constexpr int TRAFFIC_LIGHT_MARKER_SIZE = 100;
}
