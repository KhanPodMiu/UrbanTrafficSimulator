#include "render/RenderHelpers.hpp"
#include "visualization/camera.hpp"
#include <cmath>
#include <algorithm>

namespace RenderHelpers
{

Vector2 applyCamera(const Vector2& worldPos, const Camera& camera)
{
    float zoom = camera.getZoom();
    float screenX = (worldPos.x - camera.getX()) * zoom;
    float screenY = (worldPos.y - camera.getY()) * zoom;
    return Vector2(screenX, screenY);
}

SDL_Point rotatePoint(
    float localX,
    float localY,
    float centerX,
    float centerY,
    double angleDegrees)
{
    const double radians = angleDegrees * M_PI / 180.0;
    const double cosAngle = std::cos(radians);
    const double sinAngle = std::sin(radians);

    return SDL_Point{
        static_cast<int>(std::lround(
            centerX + localX * cosAngle - localY * sinAngle)),
        static_cast<int>(std::lround(
            centerY + localX * sinAngle + localY * cosAngle))
    };
}

void drawCircle(
    SDL_Renderer* renderer,
    int centerX,
    int centerY,
    int radius)
{
    constexpr int SEGMENTS = 64;
    SDL_Point points[SEGMENTS + 1];

    for (int i = 0; i <= SEGMENTS; ++i)
    {
        const double angle =
            2.0 * M_PI * static_cast<double>(i) /
            static_cast<double>(SEGMENTS);
        points[i] = SDL_Point{
            centerX + static_cast<int>(
                std::lround(radius * std::cos(angle))),
            centerY + static_cast<int>(
                std::lround(radius * std::sin(angle)))
        };
    }

    SDL_RenderDrawLines(renderer, points, SEGMENTS + 1);
}

void fillCircle(
    SDL_Renderer* renderer,
    int centerX,
    int centerY,
    int radius)
{
    for (int y = -radius; y <= radius; ++y)
    {
        const int halfWidth = static_cast<int>(std::sqrt(
            static_cast<double>(radius * radius - y * y)));
        SDL_RenderDrawLine(
            renderer,
            centerX - halfWidth,
            centerY + y,
            centerX + halfWidth,
            centerY + y);
    }
}

void drawThickLine(
    SDL_Renderer* renderer,
    float x1,
    float y1,
    float x2,
    float y2,
    float thickness)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f)
        return;

    float nx = -dy / len;
    float ny = dx / len;

    int steps = std::max(1, static_cast<int>(std::ceil(thickness)));
    float startOffset = -thickness * 0.5f;

    for (int i = 0; i <= steps; ++i)
    {
        float offset = startOffset + (thickness * static_cast<float>(i) / static_cast<float>(steps));
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>(std::lround(x1 + nx * offset)),
            static_cast<int>(std::lround(y1 + ny * offset)),
            static_cast<int>(std::lround(x2 + nx * offset)),
            static_cast<int>(std::lround(y2 + ny * offset)));
    }
}

void drawArrowHead(
    SDL_Renderer* renderer,
    float x,
    float y,
    float dirX,
    float dirY,
    float size)
{
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len < 0.001f)
        return;

    float dx = dirX / len;
    float dy = dirY / len;
    float nx = -dy;
    float ny = dx;

    float baseCentralX = x - dx * size;
    float baseCentralY = y - dy * size;

    float p1x = baseCentralX + nx * (size * 0.45f);
    float p1y = baseCentralY + ny * (size * 0.45f);
    float p2x = baseCentralX - nx * (size * 0.45f);
    float p2y = baseCentralY - ny * (size * 0.45f);

    SDL_Point points[4] = {
        SDL_Point{static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y))},
        SDL_Point{static_cast<int>(std::lround(p1x)), static_cast<int>(std::lround(p1y))},
        SDL_Point{static_cast<int>(std::lround(p2x)), static_cast<int>(std::lround(p2y))},
        SDL_Point{static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y))}
    };

    SDL_RenderDrawLines(renderer, points, 4);

    drawThickLine(renderer, p1x, p1y, x, y, 2.0f);
    drawThickLine(renderer, p2x, p2y, x, y, 2.0f);
    drawThickLine(renderer, p1x, p1y, p2x, p2y, 2.0f);
}

void renderFloatingPanel(
    SDL_Renderer* renderer,
    const SDL_Rect& rectangle,
    SDL_Color background,
    SDL_Color border)
{
    const SDL_Rect shadow{
        rectangle.x + 3,
        rectangle.y + 4,
        rectangle.w,
        rectangle.h};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 3, 12, 18, 105);
    SDL_RenderFillRect(renderer, &shadow);
    SDL_SetRenderDrawColor(
        renderer,
        background.r,
        background.g,
        background.b,
        background.a);
    SDL_RenderFillRect(renderer, &rectangle);
    SDL_SetRenderDrawColor(
        renderer,
        border.r,
        border.g,
        border.b,
        border.a);
    SDL_RenderDrawRect(renderer, &rectangle);
}

void renderTextureAt(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    int x,
    int y)
{
    if (texture == nullptr)
        return;

    SDL_Rect destination{x, y, 0, 0};
    SDL_QueryTexture(
        texture,
        nullptr,
        nullptr,
        &destination.w,
        &destination.h);
    SDL_RenderCopy(renderer, texture, nullptr, &destination);
}

void renderSolidRoadOverlay(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    const Vector2& start,
    float length,
    float width,
    double angle)
{
    if (length <= 0.0f || width <= 0.0f)
        return;

    SDL_Rect destination{
        static_cast<int>(std::lround(start.x)),
        static_cast<int>(std::lround(start.y - width * 0.5f)),
        std::max(1, static_cast<int>(std::ceil(length))),
        std::max(1, static_cast<int>(std::ceil(width)))};
    const SDL_Point pivot{0, destination.h / 2};
    SDL_RenderCopyEx(
        renderer,
        texture,
        nullptr,
        &destination,
        angle,
        &pivot,
        SDL_FLIP_NONE);
}

SDL_Texture* createTextTexture(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (surface == nullptr)
        return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture* createWhitePixelTexture(SDL_Renderer* renderer)
{
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        1,
        1,
        32,
        SDL_PIXELFORMAT_RGBA32);
    if (surface == nullptr)
        return nullptr;

    SDL_FillRect(
        surface,
        nullptr,
        SDL_MapRGBA(surface->format, 255, 255, 255, 255));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture != nullptr)
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}

} // namespace RenderHelpers
