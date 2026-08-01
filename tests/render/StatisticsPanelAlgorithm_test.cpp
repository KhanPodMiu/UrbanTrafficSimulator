#include <gtest/gtest.h>

#include "render/StatisticsPanel.hpp"

namespace
{
SDL_Event createLeftMouseUpEvent(int x, int y)
{
    SDL_Event event{};
    event.type = SDL_MOUSEBUTTONUP;
    event.button.button = SDL_BUTTON_LEFT;
    event.button.x = x;
    event.button.y = y;
    return event;
}
}

TEST(StatisticsPanelAlgorithmTest, UsesDijkstraByDefault)
{
    StatisticsPanel panel;

    EXPECT_EQ(
        panel.getSelectedAlgorithm(),
        RoutingAlgorithm::DIJKSTRA);
}

TEST(StatisticsPanelAlgorithmTest, MapsButtonsInVisualOrder)
{
    StatisticsPanel panel;

    SDL_Event event = createLeftMouseUpEvent(100, 500);
    EXPECT_EQ(
        panel.handleEvent(event),
        PanelCommand::AlgorithmChanged);
    EXPECT_EQ(
        panel.getSelectedAlgorithm(),
        RoutingAlgorithm::BFS);

    event = createLeftMouseUpEvent(220, 500);
    EXPECT_EQ(
        panel.handleEvent(event),
        PanelCommand::AlgorithmChanged);
    EXPECT_EQ(
        panel.getSelectedAlgorithm(),
        RoutingAlgorithm::DIJKSTRA);

    event = createLeftMouseUpEvent(330, 500);
    EXPECT_EQ(
        panel.handleEvent(event),
        PanelCommand::AlgorithmChanged);
    EXPECT_EQ(
        panel.getSelectedAlgorithm(),
        RoutingAlgorithm::AStarStrategy);
}

TEST(StatisticsPanelAlgorithmTest, HitboxesMeetWithoutOverlap)
{
    StatisticsPanel panel;

    SDL_Event event = createLeftMouseUpEvent(171, 500);
    EXPECT_EQ(
        panel.handleEvent(event),
        PanelCommand::AlgorithmChanged);
    EXPECT_EQ(
        panel.getSelectedAlgorithm(),
        RoutingAlgorithm::BFS);

    event = createLeftMouseUpEvent(172, 500);
    EXPECT_EQ(
        panel.handleEvent(event),
        PanelCommand::AlgorithmChanged);
    EXPECT_EQ(
        panel.getSelectedAlgorithm(),
        RoutingAlgorithm::DIJKSTRA);
}
