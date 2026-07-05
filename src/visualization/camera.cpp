#include "visualization/camera.hpp"
#include "utils/vector2i.hpp"
#include "core/Constants.hpp"

// NOTE: Config::VIEW_PORT_WIDTH/HEIGHT and MAP_WIDTH/HEIGHT are declared as
// int in Constants.hpp. The original camera.cpp declared its own copies as
// float, so plain constant-to-constant divisions here are explicitly cast
// to float to reproduce the exact same floating point result as before.
constexpr float MINZOOM = static_cast<float>(Config::VIEW_PORT_WIDTH) / static_cast<float>(Config::MAP_WIDTH);

float Camera::getX() const { return x; }
float Camera::getY() const { return y; }
float Camera::getZoom() const { return zoom; }

void Camera::addX() {
    float maxX = Config::MAP_WIDTH - Config::VIEW_PORT_WIDTH / zoom;
    if (x < maxX)
        x += Config::MOVE_SPEED / zoom;
}

void Camera::addY() {
    float maxY = Config::MAP_HEIGHT - Config::VIEW_PORT_HEIGHT / zoom;
    if (y < maxY)
        y += Config::MOVE_SPEED / zoom;
}

void Camera::subX() {
    if (x > 0)
        x -= Config::MOVE_SPEED / zoom;
    if (x < 0) x = 0;
}

void Camera::subY() {
    if (y > 0)
        y -= Config::MOVE_SPEED / zoom;
    if (y < 0) y = 0;
}

void Camera::zoomOut() {
    zoom -= zoom * 0.05f;
    if (zoom < MINZOOM) zoom = MINZOOM;
    clampPosition();
}

void Camera::zoomIn() {
    zoom += zoom * 0.05f;
    if (zoom > 3.0f) zoom = 3.0f;
    clampPosition();
}

Camera::Camera() : x(0), y(0), zoom(1.0f) {}

void Camera::clampPosition() {
    float maxX = Config::MAP_WIDTH - Config::VIEW_PORT_WIDTH / zoom;
    float maxY = Config::MAP_HEIGHT - Config::VIEW_PORT_HEIGHT / zoom;

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > maxX) x = maxX;
    if (y > maxY) y = maxY;
}

void Camera::setZoom(float newZoom)
{
    // NOTE: this local MINZOOM intentionally shadows the file-scope MINZOOM
    // above (used by zoomIn/zoomOut) exactly as in the original code — it is
    // computed from height instead of width. Preserved as-is, not unified,
    // to avoid changing existing behavior.
    float MINZOOM = static_cast<float>(Config::VIEW_PORT_HEIGHT) / static_cast<float>(Config::MAP_HEIGHT);
    if (newZoom < MINZOOM) newZoom = MINZOOM;
    if (newZoom > 3.0f)   newZoom = 3.0f;
    zoom = newZoom;
    clampPosition();
}