#include <gfx/imgui/gfx.hpp>
#include <gfx/imgui/imgui.h>
#include <gfx/imgui/imgui_internal.h>
#include <spdlog/spdlog.h>

struct BlitMultisampleData {
    gfx::Framebuffer* fbo;
    glm::vec2 point_min;
    glm::vec2 point_max;
};

static BlitMultisampleData blit_multisample_data;
void blit_multisample(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
    BlitMultisampleData* data = &blit_multisample_data;

    float height = ImGui::GetDrawData()->DisplaySize.y;

    glm::vec2 va(data->point_min.x, height - data->point_max.y);
    glm::vec2 vb(data->point_max.x, height - data->point_min.y);

    data->fbo->blit_colors({ 0, 0 }, vb - va, va, vb);
}

namespace ImGui {

bool BeginFramebuffer(
    const char* window_title,
    gfx::Framebuffer& fbo,
    bool* open,
    ImGuiWindowFlags flags)
{
    bool ret = false;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
    if (! ImGui::Begin(window_title, open, flags)) {
        ImGui::PopStyleVar();
        ImGui::End();
        return false;
    }
    ImGui::PopStyleVar();

    glm::vec2 size = ImGui::GetCurrentWindow()->ContentRegionRect.GetSize();
    glm::vec2 va = ImGui::GetCursorScreenPos();

    blit_multisample_data = BlitMultisampleData { &fbo, va, va + size };
    ImGui::GetWindowDrawList()->AddCallback(blit_multisample, nullptr);

    if (! (ImGui::GetIO().KeyMods & ImGuiKeyModFlags_Alt)) {
        ImGui::InvisibleButton("3D Window", size,
            ImGuiMouseButton_Right
                | ImGuiMouseButton_Left
                | ImGuiMouseButton_Middle);
    } else {
        ImGui::Text("");
    }

    return true;
}

void EndFramebuffer()
{
    ImGui::End();
}

void BeginMainWindow(ImGuiWindowFlags flags)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("App", nullptr, window_flags);

    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspace_id);
}

void EndMainWindow()
{
    ImGui::End();
}

glm::vec2 GetWindowContentSize()
{
    return (glm::vec2)ImGui::GetWindowContentRegionMax() - (glm::vec2)ImGui::GetWindowContentRegionMin();
}

// https://github.com/ocornut/imgui/issues/1901
bool Spinner(const char* label, float radius, int thickness)
{

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImU32& color = ImGui::GetColorU32(ImGuiCol_Button);
    
    ImGuiContext* g = ImGui::GetCurrentContext();
    const ImGuiStyle& style = g->Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius)*2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    // Render
    window->DrawList->PathClear();

    int num_segments = 30;
    int start = abs(std::sin(g->Time * 1.8f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + std::cos(a + g->Time * 8) * radius,
            centre.y + std::sin(a + g->Time * 8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
    return true;
}
}