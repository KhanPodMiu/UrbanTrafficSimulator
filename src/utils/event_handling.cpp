#include "utils/event_handling.hpp"

void handleInput(SDL_Event& event,bool& isRunning, Camera& camera)
{
    if (event.type == SDL_QUIT)
    {
        isRunning = false;
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