#include <gfx/font_awesome.hpp>
#include <gfx/framebuffer.hpp>
#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/imgui/imfilebrowser.h>
#include <gfx/imgui/imgui.h>
#include <gfx/imgui/gfx.hpp>
#include <gfx/texture.hpp>
#include <gfx/vertex_array.hpp>
#include "scene.hpp"


int main()
{
    gfx::Gfx app(gfx::InitOptions {
        .title = "ImGui Test",
        .maximized = true,
        .resizable = true,
        .imgui = true,
        .debug_context = true });

    ImGui::FileBrowser browser;

    browser.SetTitle("Open File...");
    browser.SetTypeFilters({ ".txt" });

    Scene scene;
    bool styles = false;

    app.main_loop([&]() {
        ImGui::BeginMainWindow();

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "\tOpen File...", "CTRL+O")) {
                    browser.Open();
                }

                if (ImGui::MenuItem(ICON_FA_SAVE "\tSave File", "CTRL+S")) {
                    browser.Open();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem(ICON_FA_PALETTE "\tStyles...")) {
                    styles = true;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        scene.imgui_window();

        browser.Display();

        if (browser.HasSelected()) {
            browser.ClearSelected();
        }

        ImGui::ShowDemoWindow();

        if (styles) {
            ImGui::Begin("Style Editor", &styles);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }
        ImGui::EndMainWindow();

        app.draw_gui();
    });

    return 0;
}
