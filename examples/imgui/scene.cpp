#include "scene.hpp"
#include "shaders.h"
#include <gfx/imgui/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pass.hpp>


const gfx::VertexArray::Layout Vertex::layout = {
    { Attrib::Position, 3, gfx::Type::Float },
    { Attrib::Normal, 3, gfx::Type::Float },
    { Attrib::Color, 3, gfx::Type::Float },
};

Scene::Scene()
    : cube()
    , vertices()
    , indices()
    , framebuffer()
    , lens(70.0, 1.0, 0.2, 10.2)
    , camera(lens)
    , render_pipeline(nullptr)
    , size(0, 0)
{
    framebuffer
        .add_color_buffer(glm::ivec2(2048, 2048), gfx::TextureType::Rgba, 4)
        .set_depth_buffer(glm::ivec2(2048, 2048), 4);

    camera.with_position({ 0.0, 0.0, 5.0 })
        .look_at({ 0.0, 0.0, 0.0 })
        .with_up_vector({ 0.0, 1.0, 0.0 });

    render_pipeline
        = gfx::RenderPipeline::Builder()
              .with_shader(gfx::ShaderProgram::Builder()
                               .register_class<gfx::CameraLens>()
                               .register_class<gfx::CameraRig>()
                               .with_vertex_shader(VS)
                               .with_fragment_shader(FS)
                               .build())
              .build_unique();

    vertices.update_buffer(
        { { { -1, -1, 1 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
            { { 1, -1, 1 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
            { { 1, 1, 1 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
            { { -1, 1, 1 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },

            { { -1, 1, -1 }, { 0.0, 0.0, -1.0 }, { 0.0, 0.0, 1.0 } },
            { { 1, 1, -1 }, { 0.0, 0.0, -1.0 }, { 0.0, 0.0, 1.0 } },
            { { 1, -1, -1 }, { 0.0, 0.0, -1.0 }, { 0.0, 0.0, 1.0 } },
            { { -1, -1, -1 }, { 0.0, 0.0, -1.0 }, { 0.0, 0.0, 1.0 } },

            { { 1, -1, -1 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { 1, 1, -1 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { 1, 1, 1 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { 1, -1, 1 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },

            { { -1, -1, 1 }, { -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { -1, 1, 1 }, { -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { -1, 1, -1 }, { -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { -1, -1, -1 }, { -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },

            { { 1, 1, -1 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
            { { -1, 1, -1 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
            { { -1, 1, 1 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
            { { 1, 1, 1 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } },

            { { 1, -1, 1 }, { 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
            { { -1, -1, 1 }, { 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
            { { -1, -1, -1 }, { 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
            { { 1, -1, -1 }, { 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 } } });

    indices.update_buffer({
        0,
        1,
        2,
        2,
        3,
        0,
        4,
        5,
        6,
        6,
        7,
        4,
        8,
        9,
        10,
        10,
        11,
        8,
        12,
        13,
        14,
        14,
        15,
        12,
        16,
        17,
        18,
        18,
        19,
        16,
        20,
        21,
        22,
        22,
        23,
        20,
    });

    cube.add_buffer(Vertex::layout, vertices)
        .set_indices_buffer(indices)
        .set_mode(gfx::Mode::Triangles);
}


void Scene::render(const glm::ivec2& paint_size)
{
    if (paint_size != size) {
        lens.resize_event(paint_size);
        size = paint_size;
    }

    glEnable(GL_DEPTH_TEST);
    gfx::RenderPass(framebuffer, gfx::ClearOperation::color_and_depth({ 0.3, 0.3, 0.3, 1.0 }))
        .viewport({0, 0}, paint_size)
        .set_pipeline(*render_pipeline)
        .with_camera(camera)
        .draw(cube);
}

void Scene::imgui_window()
{
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    if (ImGui::BeginFramebuffer("Example 3D render", framebuffer)) {
        if (ImGui::IsItemActive()) {
            glm::vec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0);
            camera.orbit(drag.x * 0.01);
            camera.orbit_vertical(drag.y * 0.01);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

            drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right, 0.0);
            camera.truck(-drag.x * 0.01);
            camera.crane(drag.y * 0.01);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);

        }

        if (ImGui::IsItemHovered()) {
            lens.zoom(1.0 + ImGui::GetIO().MouseWheel * 0.001);
        }

        this->render(ImGui::GetWindowContentSize());

        ImGui::EndFramebuffer();
    }
}