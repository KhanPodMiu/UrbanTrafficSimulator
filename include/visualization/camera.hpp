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

    void setX(float newX);
    void setY(float newY);
    void offsetPosition(float dx, float dy);

    void zoomIn(float centerX = -1, float centerY = -1);
    void zoomOut(float centerX = -1, float centerY = -1);

    void addX();
    void addY();
    void subX();
    void subY();

    void setZoom(float newZoom);

    Camera();
};