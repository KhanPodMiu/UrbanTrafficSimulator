#include <gtest/gtest.h>

#include "core/Constants.hpp"
#include "render/TrafficHeatMap.hpp"
#include "visualization/VisualizationEngine.hpp"

TEST(TrafficHeatMapScaleTest, ClassifiesBoundaryValues)
{
    EXPECT_EQ(TrafficHeatMapScale::classify(0), TrafficFlowLevel::Low);
    EXPECT_EQ(TrafficHeatMapScale::classify(39), TrafficFlowLevel::Low);
    EXPECT_EQ(
        TrafficHeatMapScale::classify(40),
        TrafficFlowLevel::Moderate);
    EXPECT_EQ(
        TrafficHeatMapScale::classify(69),
        TrafficFlowLevel::Moderate);
    EXPECT_EQ(
        TrafficHeatMapScale::classify(70),
        TrafficFlowLevel::Heavy);
    EXPECT_EQ(
        TrafficHeatMapScale::classify(100),
        TrafficFlowLevel::Heavy);
}

TEST(TrafficHeatMapScaleTest, HandlesOutOfRangeValuesSafely)
{
    EXPECT_EQ(TrafficHeatMapScale::classify(-1), TrafficFlowLevel::Low);
    EXPECT_EQ(TrafficHeatMapScale::classify(101), TrafficFlowLevel::Heavy);
}

TEST(TrafficHeatMapScaleTest, UsesAccessibleTrafficPalette)
{
    const SDL_Color low = TrafficHeatMapScale::colorFor(0);
    const SDL_Color moderate = TrafficHeatMapScale::colorFor(50);
    const SDL_Color heavy = TrafficHeatMapScale::colorFor(100);

    EXPECT_EQ(low.r, 34);
    EXPECT_EQ(low.g, 197);
    EXPECT_EQ(low.b, 94);

    EXPECT_EQ(moderate.r, 245);
    EXPECT_EQ(moderate.g, 158);
    EXPECT_EQ(moderate.b, 11);

    EXPECT_EQ(heavy.r, 239);
    EXPECT_EQ(heavy.g, 68);
    EXPECT_EQ(heavy.b, 68);
}

TEST(TrafficHeatMapInteractionTest, KeyboardShortcutTogglesOverlay)
{
    VisualizationEngine visualization;
    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_h;
    event.key.repeat = 0;

    EXPECT_TRUE(visualization.handleTrafficHeatMapEvent(event));
    EXPECT_TRUE(visualization.isTrafficHeatMapEnabled());
    EXPECT_TRUE(visualization.handleTrafficHeatMapEvent(event));
    EXPECT_FALSE(visualization.isTrafficHeatMapEnabled());
}

TEST(TrafficHeatMapInteractionTest, ButtonClickIsConsumedAndTogglesOverlay)
{
    VisualizationEngine visualization;
    SDL_Event mouseDown{};
    mouseDown.type = SDL_MOUSEBUTTONDOWN;
    mouseDown.button.button = SDL_BUTTON_LEFT;
    mouseDown.button.x = Config::PANEL_WIDTH + 20;
    mouseDown.button.y = 20;

    SDL_Event mouseUp = mouseDown;
    mouseUp.type = SDL_MOUSEBUTTONUP;

    EXPECT_TRUE(visualization.handleTrafficHeatMapEvent(mouseDown));
    EXPECT_FALSE(visualization.isTrafficHeatMapEnabled());
    EXPECT_TRUE(visualization.handleTrafficHeatMapEvent(mouseUp));
    EXPECT_TRUE(visualization.isTrafficHeatMapEnabled());
}

TEST(TrafficHeatMapInteractionTest, ClickOutsideButtonIsIgnored)
{
    VisualizationEngine visualization;
    SDL_Event event{};
    event.type = SDL_MOUSEBUTTONDOWN;
    event.button.button = SDL_BUTTON_LEFT;
    event.button.x = Config::PANEL_WIDTH + 300;
    event.button.y = 300;

    EXPECT_FALSE(visualization.handleTrafficHeatMapEvent(event));
    EXPECT_FALSE(visualization.isTrafficHeatMapEnabled());
}
