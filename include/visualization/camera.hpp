#pragma once

#include "utils/vector2i.hpp"

class Camera
{
private:
    float x;
    float y;
    float zoom;
    void clampPosition();
public:

    float getX() const;
    float getY() const;
    float getZoom() const;

    void zoomIn();
    void zoomOut();

    void addX();
    void addY();
    void subX();
    void subY();

    void setZoom(float newZoom);

    Camera();
};