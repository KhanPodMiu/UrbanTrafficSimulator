#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <memory>
#include <string>

class Intersection;
class RenderWindow;
class Vehicle;

enum class SelectedEntityType
{
    None,
    Vehicle,
    Intersection
};

class EntityInfoPanel
{
public:
    EntityInfoPanel();
    ~EntityInfoPanel();

    bool loadAssets();
    void selectVehicle(const std::shared_ptr<Vehicle>& vehicle);
    void selectIntersection(
        const std::shared_ptr<Intersection>& intersection);
    void clearSelection();

    std::shared_ptr<Vehicle> getSelectedVehicle() const;
    std::shared_ptr<Intersection> getSelectedIntersection() const;
    bool containsPoint(int x, int y) const;

    void render(RenderWindow& window);
    void cleanUp();

private:
    void renderVehicleInfo(
        RenderWindow& window,
        const std::shared_ptr<Vehicle>& vehicle);
    void renderIntersectionInfo(
        RenderWindow& window,
        const std::shared_ptr<Intersection>& intersection);
    void renderPanelBackground(RenderWindow& window, int height);
    void renderText(
        RenderWindow& window,
        TTF_Font* font,
        const std::string& text,
        int x,
        int y,
        SDL_Color color);

    SelectedEntityType selectedType_;
    std::weak_ptr<Vehicle> selectedVehicle_;
    std::weak_ptr<Intersection> selectedIntersection_;
    TTF_Font* titleFont_;
    TTF_Font* bodyFont_;
    SDL_Rect panelRect_;
};
