#include "visualization/camera.hpp"

constexpr int VIEW_PORT_WIDTH = 1200;
constexpr int VIEW_PORT_HEIGHT = 900;
constexpr int MAP_WIDTH = 4000;
constexpr int MAP_HEIGHT = 4000;

float Camera::getX(){
    return x;
}

float Camera::getY(){
    return y;
} 

float Camera::getZoom(){
    return zoom;
}

void Camera::addX(){
    if(x < MAP_WIDTH - VIEW_PORT_WIDTH){
        x += 20;
    }
}

void Camera::addY(){
    if(y < MAP_HEIGHT - VIEW_PORT_HEIGHT){
        y += 20;
    }
}

void Camera::subX(){
    if(x != 0){
        x -= 20;
    }
}

void Camera::subY(){
    if(y != 0){
        y -= 20;
    }
}

Camera::Camera() : x(0), y(0), zoom(1.0){

}

void Camera::move(int dx, int dy){
    x += dx;
    y += dy;
}