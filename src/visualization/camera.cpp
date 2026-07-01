#include "visualization/camera.hpp"
#include "utils/vector2i.hpp"

constexpr float VIEW_PORT_WIDTH  = 1200;
constexpr float VIEW_PORT_HEIGHT = 900;
constexpr float MAP_WIDTH        = 53000.0;
constexpr float MAP_HEIGHT       = 40000.0;
constexpr float MOVE_SPEED       = 40.0f;

constexpr float MINZOOM = VIEW_PORT_WIDTH/MAP_WIDTH;

float Camera::getX() const { return x; }
float Camera::getY() const { return y; }
float Camera::getZoom() const { return zoom; }

void Camera::addX() {
    float maxX = MAP_WIDTH - VIEW_PORT_WIDTH / zoom;
    if (x < maxX)
        x += MOVE_SPEED / zoom;
}

void Camera::addY() {
    float maxY = MAP_HEIGHT - VIEW_PORT_HEIGHT / zoom;
    if (y < maxY)
        y += MOVE_SPEED / zoom;
}

void Camera::subX() {
    if (x > 0)
        x -= MOVE_SPEED / zoom;
    if (x < 0) x = 0;
}

void Camera::subY() {
    if (y > 0)
        y -= MOVE_SPEED / zoom;
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
    float maxX = MAP_WIDTH - VIEW_PORT_WIDTH / zoom;
    float maxY = MAP_HEIGHT - VIEW_PORT_HEIGHT / zoom;

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > maxX) x = maxX;
    if (y > maxY) y = maxY;
}

void Camera::setZoom(float newZoom)
{
    float MINZOOM = VIEW_PORT_HEIGHT / MAP_HEIGHT;
    if (newZoom < MINZOOM) newZoom = MINZOOM;
    if (newZoom > 3.0f)   newZoom = 3.0f;
    zoom = newZoom;
    clampPosition();
}