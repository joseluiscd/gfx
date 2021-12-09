#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pass.hpp>
#include <gfx/render_pipeline.hpp>
#include <gfx/vertex_array.hpp>

const static char* VS = R"(
layout (location = kPosition) in vec2 v_position;
layout (location = kColor) in vec3 v_color;

out vec3 i_color;

void main()
{
    gl_Position = vec4(v_position, 0.0, 1.0);
    i_color = v_color;
}
)";

const static char* FS = R"(
in vec3 i_color;

out vec4 f_color;

void main()
{
    f_color = vec4(i_color, 1.0);
}
)";

enum Attrib : unsigned int {
    Position = 0,
    Color,
};

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    GFX_VERTEX_LAYOUT(
        { Attrib::Position, 2, gfx::Type::Float },
        { Attrib::Color, 3, gfx::Type::Float },
    );
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
        .add_buffer(vertices.data(), vertices.size())
        .set_indices_buffer(indices, 3)
        .set_mode(gfx::Mode::Triangles);

    gfx::RenderPipeline pipeline = gfx::RenderPipeline::Builder("example")
                                       .with_shader(gfx::ShaderProgram::Builder("example")
                                                        .with_constant("kPosition", Attrib::Position)
                                                        .with_constant("kColor", Attrib::Color)
                                                        .with_vertex_shader(VS)
                                                        .with_fragment_shader(FS)
                                                        .build())
                                       .build();

    app.main_loop([&]() {
        gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color())
            .set_pipeline(pipeline)
            .draw(triangle);
    });

    return 0;
}
