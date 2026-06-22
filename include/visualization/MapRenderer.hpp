#pragma once

class Graph;
class RenderWindow;

class MapRenderer {
public:
    void render(RenderWindow& window, const Graph& graph);
};