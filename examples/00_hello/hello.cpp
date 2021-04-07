#define GFX_VALIDATION

#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pipeline.hpp>
#include <gfx/vertex_array.hpp>
#include <gfx/render_pass.hpp>
#include <spdlog/spdlog.h>

enum Attrib : unsigned int {
    Position = 0,
    Color = 1,
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
    gfx::Gfx app(gfx::InitOptions {
        .title = "Hello world",
        .resizable = false });

    std::vector<Vertex> vertices = {
        { glm::vec2(-0.5, -0.5), glm::vec3(1.0, 0.0, 0.0) },
        { glm::vec2(0.5, -0.5), glm::vec3(0.0, 1.0, 0.0) },
        { glm::vec2(0.0, 0.5), glm::vec3(0.0, 0.0, 1.0) },
    };

    unsigned int indices[3] = { 0, 1, 2 };

    gfx::VertexArray triangle;
    triangle
        .add_buffer(Vertex::layout, vertices.data(), vertices.size())
        .set_indices_buffer(indices, 3)
        .set_mode(gfx::Mode::Triangles);

    gfx::RenderPipeline pipeline = gfx::RenderPipeline::Builder("example")
                                       .with_shader(gfx::ShaderProgram::Builder("example")
                                                        .with_vertex_shader_file("example.vs")
                                                        .with_fragment_shader_file("example.fs")
                                                        .build())
                                       .build();

    app.main_loop([&]() {
        gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color())
            .set_pipeline(pipeline)
            .draw(triangle);
        
    });

    return 0;
}
