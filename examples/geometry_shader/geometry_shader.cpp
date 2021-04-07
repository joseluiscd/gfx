#define GFX_VALIDATION

#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pipeline.hpp>
#include <gfx/vertex_array.hpp>
#include <spdlog/spdlog.h>
#include <gfx/render_pass.hpp>

enum Attrib : unsigned int {
    Position = 0,
    Color = 10,
};

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static const gfx::VertexArray::Layout layout;
};

const gfx::VertexArray::Layout Vertex::layout = {
    { Attrib::Position, 2, gfx::Type::Float },
    { Attrib::Color, 3, gfx::Type::Float },
};

int main()
{
    gfx::Gfx app(
        gfx::InitOptions {
            .title = "Hello world",
            .resizable = false });

    std::vector<Vertex> vertices = {
        { glm::vec2(-0.5, -0.5), glm::vec3(0.5, 0.0, 0.0) },
        { glm::vec2(0.5, -0.5), glm::vec3(0.0, 0.5, 0.0) },
        { glm::vec2(0.0, 0.5), glm::vec3(0.0, 0.0, 0.5) }
    };

    unsigned int indices[3] = { 0, 1, 2 };

    gfx::VertexArray triangle;
    triangle
        .add_buffer(Vertex::layout, vertices.data(), vertices.size())
        .set_mode(gfx::Mode::Points);

    gfx::RenderPipeline pipeline = gfx::RenderPipeline::Builder("example")
                                       .with_shader(gfx::ShaderProgram::Builder("example")
                                                        .with_constant("kPosition", Attrib::Position)
                                                        .with_constant("kColor", Attrib::Color)
                                                        .with_vertex_shader_file("example.vs")
                                                        .with_geometry_shader_file("example.gs")
                                                        .with_fragment_shader_file("example.fs")
                                                        .build())
                                       .build();

    app.main_loop([&]() {
        gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color_and_depth())
            .set_pipeline(pipeline)
            .draw(triangle);
    });

    return 0;
}
