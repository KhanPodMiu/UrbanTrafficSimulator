#include "utils/event_handling.hpp"

void handleInput(SDL_Event& event,bool& isRunning, Camera& camera)
{
    if (event.type == SDL_QUIT)
    {
        isRunning = false;
        return;
    }

    static bool isDragging = false;
    static int lastMouseX = 0;
    static int lastMouseY = 0;

    if (event.type == SDL_MOUSEWHEEL)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        if (event.wheel.y > 0) {
            camera.zoomIn(static_cast<float>(mouseX), static_cast<float>(mouseY));
        } else if (event.wheel.y < 0) {
            camera.zoomOut(static_cast<float>(mouseX), static_cast<float>(mouseY));
        }
        return;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging = true;
            lastMouseX = event.button.x;
            lastMouseY = event.button.y;
        }
        return;
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_MIDDLE)
        {
            isDragging = false;
        }
        return;
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        if (isDragging)
        {
            int dx = event.motion.x - lastMouseX;
            int dy = event.motion.y - lastMouseY;
            
            // Pan camera opposite to mouse drag direction, scaled by zoom
            camera.offsetPosition(-static_cast<float>(dx) / camera.getZoom(), -static_cast<float>(dy) / camera.getZoom());
            
            lastMouseX = event.motion.x;
            lastMouseY = event.motion.y;
        }
        return;
    }

    if (event.type != SDL_KEYDOWN)
    {
        return;
    }

    switch(event.key.keysym.sym)
    {
        case SDLK_ESCAPE:
            isRunning = false;
            break;

        case SDLK_w:
            camera.subY();
            break;

        case SDLK_a:
            camera.subX();
            break;

        case SDLK_s:
            camera.addY();
            break;

        case SDLK_d:
            camera.addX();
            break;

        case SDLK_q:
            camera.zoomOut();
            break;

        case SDLK_e:
            camera.zoomIn();
            break;
    }
}