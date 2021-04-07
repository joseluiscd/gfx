#pragma once
#include <gfx/framebuffer.hpp>
#include <gfx/imgui/imgui.h>

namespace ImGui {

bool BeginFramebuffer(
    const char* window_title,
    gfx::Framebuffer& fbo,
    bool* open = nullptr,
    ImGuiWindowFlags flags = 0);

void EndFramebuffer();

void BeginMainWindow(ImGuiWindowFlags flags = 0);
void EndMainWindow();

glm::vec2 GetWindowContentSize();

bool Spinner(const char* label, float radius = 24.0f, int thickness = 2.0f);

}