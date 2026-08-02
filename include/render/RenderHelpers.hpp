#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "utils/vector2i.hpp"

class Camera;

namespace RenderHelpers
{
    Vector2 applyCamera(const Vector2& worldPos, const Camera& camera);

    SDL_Point rotatePoint(
        float localX,
        float localY,
        float centerX,
        float centerY,
        double angleDegrees);

    void drawCircle(
        SDL_Renderer* renderer,
        int centerX,
        int centerY,
        int radius);

    void fillCircle(
        SDL_Renderer* renderer,
        int centerX,
        int centerY,
        int radius);

    void drawThickLine(
        SDL_Renderer* renderer,
        float x1,
        float y1,
        float x2,
        float y2,
        float thickness);

    void drawArrowHead(
        SDL_Renderer* renderer,
        float x,
        float y,
        float dirX,
        float dirY,
        float size);

    void renderFloatingPanel(
        SDL_Renderer* renderer,
        const SDL_Rect& rectangle,
        SDL_Color background,
        SDL_Color border);

    void renderTextureAt(
        SDL_Renderer* renderer,
        SDL_Texture* texture,
        int x,
        int y);

    void renderSolidRoadOverlay(
        SDL_Renderer* renderer,
        SDL_Texture* texture,
        const Vector2& start,
        float length,
        float width,
        double angle);

    SDL_Texture* createTextTexture(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const char* text,
        SDL_Color color);

    SDL_Texture* createWhitePixelTexture(SDL_Renderer* renderer);
}
