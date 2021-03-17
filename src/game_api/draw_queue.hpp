#pragma once

#include <string>
#include <vector>

#include "imgui.h"

class DrawQueue
{
public:
    DrawQueue();
    ~DrawQueue();

    void queue_line(ImVec2 start, ImVec2 end, float thickness, ImU32 color);
    void queue_rect(ImVec2 min, ImVec2 max, float thickness, float rounding, ImU32 color);
    void queue_rect_filled(ImVec2 min, ImVec2 max, float rounding, ImU32 color);
    void queue_circle(ImVec2 center, float radius, float thickness, ImU32 color);
    void queue_circle_filled(ImVec2 center, float radius, ImU32 color);
    void queue_text(ImVec2 start, std::string text, ImU32 color);

    void draw(ImDrawList* draw_list);
    void clear();

private:
    struct QueueElement;
    std::vector<QueueElement> m_Queue;
};
