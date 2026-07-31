#include "render/EntityInfoPanel.hpp"

#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "vehicles/Vehicle.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace
{
constexpr int PANEL_WIDTH = 340;
constexpr int VEHICLE_PANEL_HEIGHT = 230;
constexpr int INTERSECTION_PANEL_HEIGHT = 250;
constexpr int PANEL_MARGIN = 20;
constexpr int CONTENT_PADDING = 20;
constexpr int FIRST_LINE_Y_OFFSET = 62;
constexpr int LINE_HEIGHT = 25;

const SDL_Color TITLE_COLOR{255, 214, 48, 255};
const SDL_Color TEXT_COLOR{238, 244, 247, 255};
const SDL_Color MUTED_TEXT_COLOR{176, 198, 207, 255};

std::string vehicleTypeToString(VehicleType type)
{
    switch (type)
    {
        case VehicleType::BUS:
            return "Bus";
        case VehicleType::EMERGENCY:
            return "Emergency";
        case VehicleType::CAR:
        default:
            return "Car";
    }
}

std::string intersectionTypeToString(IntersectionType type)
{
    switch (type)
    {
        case IntersectionType::DEAD_END:
            return "Dead end";
        case IntersectionType::STRAIGHT:
            return "Straight";
        case IntersectionType::T_INTERSECTION:
            return "T-intersection";
        case IntersectionType::CROSS:
            return "Cross";
        case IntersectionType::ROUNDABOUT:
            return "Roundabout";
        default:
            return "Unknown";
    }
}

std::string valueOrUnavailable(const std::string& value)
{
    return value.empty() ? "N/A" : value;
}
}

EntityInfoPanel::EntityInfoPanel()
    : selectedType_(SelectedEntityType::None),
      titleFont_(nullptr),
      bodyFont_(nullptr),
      panelRect_{
          Config::WINDOW_WIDTH - PANEL_WIDTH - PANEL_MARGIN,
          PANEL_MARGIN,
          PANEL_WIDTH,
          0}
{
}

EntityInfoPanel::~EntityInfoPanel()
{
}

bool EntityInfoPanel::loadAssets()
{
    titleFont_ = TTF_OpenFont(
        "assets/textures/fonts/Octarine-Bold.otf",
        22);
    if (titleFont_ == nullptr)
    {
        std::cerr << "Cannot load entity panel title font: "
                  << TTF_GetError()
                  << std::endl;
        return false;
    }

    bodyFont_ = TTF_OpenFont(
        "assets/textures/fonts/Octarine-Bold.otf",
        16);
    if (bodyFont_ == nullptr)
    {
        std::cerr << "Cannot load entity panel body font: "
                  << TTF_GetError()
                  << std::endl;
        cleanUp();
        return false;
    }

    return true;
}

void EntityInfoPanel::selectVehicle(
    const std::shared_ptr<Vehicle>& vehicle)
{
    selectedVehicle_ = vehicle;
    selectedIntersection_.reset();
    selectedType_ =
        vehicle ? SelectedEntityType::Vehicle : SelectedEntityType::None;
}

void EntityInfoPanel::selectIntersection(
    const std::shared_ptr<Intersection>& intersection)
{
    selectedIntersection_ = intersection;
    selectedVehicle_.reset();
    selectedType_ = intersection
        ? SelectedEntityType::Intersection
        : SelectedEntityType::None;
}

void EntityInfoPanel::clearSelection()
{
    selectedType_ = SelectedEntityType::None;
    selectedVehicle_.reset();
    selectedIntersection_.reset();
    panelRect_.h = 0;
}

std::shared_ptr<Vehicle> EntityInfoPanel::getSelectedVehicle() const
{
    if (selectedType_ != SelectedEntityType::Vehicle)
        return nullptr;

    return selectedVehicle_.lock();
}

std::shared_ptr<Intersection>
EntityInfoPanel::getSelectedIntersection() const
{
    if (selectedType_ != SelectedEntityType::Intersection)
        return nullptr;

    return selectedIntersection_.lock();
}

bool EntityInfoPanel::containsPoint(int x, int y) const
{
    if (selectedType_ == SelectedEntityType::None || panelRect_.h <= 0)
        return false;

    const SDL_Point point{x, y};
    return SDL_PointInRect(&point, &panelRect_) == SDL_TRUE;
}

void EntityInfoPanel::render(RenderWindow& window)
{
    if (selectedType_ == SelectedEntityType::Vehicle)
    {
        const std::shared_ptr<Vehicle> vehicle = selectedVehicle_.lock();
        if (!vehicle || vehicle->isFinished())
        {
            clearSelection();
            return;
        }

        renderVehicleInfo(window, vehicle);
        return;
    }

    if (selectedType_ == SelectedEntityType::Intersection)
    {
        const std::shared_ptr<Intersection> intersection =
            selectedIntersection_.lock();
        if (!intersection)
        {
            clearSelection();
            return;
        }

        renderIntersectionInfo(window, intersection);
    }
}

void EntityInfoPanel::renderVehicleInfo(
    RenderWindow& window,
    const std::shared_ptr<Vehicle>& vehicle)
{
    panelRect_.h = VEHICLE_PANEL_HEIGHT;
    renderPanelBackground(window, panelRect_.h);

    const int textX = panelRect_.x + CONTENT_PADDING;
    int textY = panelRect_.y + FIRST_LINE_Y_OFFSET;
    renderText(
        window,
        titleFont_,
        "VEHICLE INFORMATION",
        textX,
        panelRect_.y + 18,
        TITLE_COLOR);

    const std::string source =
        valueOrUnavailable(vehicle->getSourceIntersectionId());
    const std::string destination =
        valueOrUnavailable(vehicle->getDestinationIntersectionId());
    const std::string currentRoad =
        valueOrUnavailable(vehicle->getCurrentRoadId());

    std::ostringstream speedText;
    speedText << std::fixed << std::setprecision(1)
              << "Speed: " << vehicle->getCurrentSpeed()
              << " / " << vehicle->getMaxSpeed();

    const std::size_t routeSize = vehicle->getRoute().size();
    const std::size_t currentStep = routeSize == 0
        ? 0
        : std::min(vehicle->getRouteIndex() + 1, routeSize);

    renderText(
        window,
        bodyFont_,
        "ID: " + vehicle->getId(),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Type: " + vehicleTypeToString(vehicle->getType()),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Route: " + source + " -> " + destination,
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Current road: " + currentRoad,
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        speedText.str(),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Route progress: " + std::to_string(currentStep) +
            " / " + std::to_string(routeSize),
        textX,
        textY,
        MUTED_TEXT_COLOR);
}

void EntityInfoPanel::renderIntersectionInfo(
    RenderWindow& window,
    const std::shared_ptr<Intersection>& intersection)
{
    panelRect_.h = INTERSECTION_PANEL_HEIGHT;
    renderPanelBackground(window, panelRect_.h);

    const int textX = panelRect_.x + CONTENT_PADDING;
    int textY = panelRect_.y + FIRST_LINE_Y_OFFSET;
    renderText(
        window,
        titleFont_,
        "INTERSECTION INFORMATION",
        textX,
        panelRect_.y + 18,
        TITLE_COLOR);

    bool hasTrafficLight = false;
    for (const Road* road : intersection->getIncomingRoads())
    {
        if (road != nullptr && road->isTrafficLightEnabled())
        {
            hasTrafficLight = true;
            break;
        }
    }

    const std::string intersectionType =
        intersection->getDegree() == 0
            ? "Isolated"
            : intersectionTypeToString(intersection->getType());

    renderText(
        window,
        bodyFont_,
        "ID: " + intersection->getIntersectionID(),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Type: " + intersectionType,
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Position: (" + std::to_string(intersection->getX()) +
            ", " + std::to_string(intersection->getY()) + ")",
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Incoming roads: " +
            std::to_string(intersection->getIncomingRoadCount()),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Outgoing roads: " +
            std::to_string(intersection->getOutgoingRoadCount()),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        "Degree: " + std::to_string(intersection->getDegree()),
        textX,
        textY,
        TEXT_COLOR);
    textY += LINE_HEIGHT;
    renderText(
        window,
        bodyFont_,
        std::string("Traffic light: ") +
            (hasTrafficLight ? "Yes" : "No"),
        textX,
        textY,
        MUTED_TEXT_COLOR);
}

void EntityInfoPanel::renderPanelBackground(
    RenderWindow& window,
    int height)
{
    panelRect_.h = height;
    SDL_Renderer* renderer = window.getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 18, 49, 64, 225);
    SDL_RenderFillRect(renderer, &panelRect_);
    SDL_SetRenderDrawColor(renderer, 255, 214, 48, 255);
    SDL_RenderDrawRect(renderer, &panelRect_);
}

void EntityInfoPanel::renderText(
    RenderWindow& window,
    TTF_Font* font,
    const std::string& text,
    int x,
    int y,
    SDL_Color color)
{
    if (font == nullptr)
        return;

    SDL_Surface* surface =
        TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface == nullptr)
    {
        std::cerr << "Cannot create entity info text surface: "
                  << TTF_GetError()
                  << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        window.getRenderer(),
        surface);
    if (texture == nullptr)
    {
        std::cerr << "Cannot create entity info text texture: "
                  << SDL_GetError()
                  << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    const SDL_Rect destination{x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    SDL_RenderCopy(
        window.getRenderer(),
        texture,
        nullptr,
        &destination);
    SDL_DestroyTexture(texture);
}

void EntityInfoPanel::cleanUp()
{
    if (titleFont_ != nullptr)
    {
        TTF_CloseFont(titleFont_);
        titleFont_ = nullptr;
    }

    if (bodyFont_ != nullptr)
    {
        TTF_CloseFont(bodyFont_);
        bodyFont_ = nullptr;
    }

    clearSelection();
}
