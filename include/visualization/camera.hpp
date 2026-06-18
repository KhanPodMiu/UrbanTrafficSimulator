#pragma once

class Camera
{
private:
    float x;
    float y;
    float zoom;
public:

    float getX();
    float getY();
    float getZoom();

    void addX();
    void addY();
    void subX();
    void subY();

    void move(int dx, int dy);

    Camera();
};