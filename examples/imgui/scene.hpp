#pragma once

#include <gfx/camera.hpp>
#include <gfx/framebuffer.hpp>
#include <gfx/vertex_array.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static const gfx::VertexArray::Layout layout;
};

enum Attrib : unsigned int {
    Position = 0,
    Normal = 1,
    Color = 2,
};

class Scene {
public:
    Scene();
    void render(const glm::ivec2& paint_size);
    void imgui_window();

private:
    gfx::VertexArray cube;

    gfx::Buffer<Vertex> vertices;
    gfx::Buffer<uint16_t> indices;

    gfx::Framebuffer framebuffer;
    gfx::PerspectiveCameraLens lens;
    gfx::CameraRig camera;

    std::unique_ptr<gfx::RenderPipeline> render_pipeline;
    glm::ivec2 size;
};