#pragma once

#include "utils/vector2i.hpp"

class Camera
{
private:
    float x;
    float y;
    float zoom;
public:

    float getX() const;
    float getY() const;
    float getZoom();

    void zoomIn();
    void zoomOut();

    void addX();
    void addY();
    void subX();
    void subY();

    void move(int dx, int dy);

    Camera();
};