#include "visualization/camera.hpp"
#include "utils/vector2i.hpp"

constexpr float VIEW_PORT_WIDTH  = 1200;
constexpr float VIEW_PORT_HEIGHT = 900;
constexpr float MAP_WIDTH        = 4000;
constexpr float MAP_HEIGHT       = 4000;
constexpr float MOVE_SPEED       = 20.0f;

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

void Camera::zoomIn() {
    if (zoom < 3.0f)
        zoom += 0.1f;
}

void Camera::zoomOut() {
    if (zoom > 0.2f)
        zoom -= 0.1f;
}

Camera::Camera() : x(0), y(0), zoom(1.0f) {}
