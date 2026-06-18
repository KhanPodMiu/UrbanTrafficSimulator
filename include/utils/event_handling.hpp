#pragma once

#include "visualization/camera.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

void handleInput(SDL_Event& event, bool& isRunning, Camera& camera);