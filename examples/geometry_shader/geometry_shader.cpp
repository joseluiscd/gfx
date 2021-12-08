#define GFX_VALIDATION

#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pipeline.hpp>
#include <gfx/vertex_array.hpp>
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

const char* VS = R"(
layout (location = kPosition) in vec2 v_position;
layout (location = kColor) in vec3 v_color;

out vec3 i_color;

void main()
{
    gl_Position = vec4(v_position, 0.0, 1.0);
    i_color = v_color;
}
)";

const char* GS = R"(
layout (points) in;
in vec3 i_color[];

layout (triangle_strip, max_vertices=3) out;
out vec3 v_color;

void main()
{
    vec4 position = gl_in[0].gl_Position;
    vec3 color = i_color[0];

    gl_Position = position + vec4(0.1, -0.1, 0.0, 0.0);
    v_color = color + vec3(0.5, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.1, 0.0, 0.0);
    v_color = color + vec3(0.0, 0.5, 0.0);
    EmitVertex();
    gl_Position = position + vec4(-0.1, -0.1, 0.0, 0.0);
    v_color = color + vec3(0.0, 0.0, 0.5);
    EmitVertex();

    EndPrimitive();

}
)";

const char* FS = R"(
in vec3 v_color;

out vec4 f_color;

void main()
{
    f_color = vec4(v_color, 1.0);
}
)";

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
                                                        .with_vertex_shader(VS)
                                                        .with_geometry_shader(GS)
                                                        .with_fragment_shader(FS)
                                                        .build())
                                       .build();

    app.main_loop([&]() {
        gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color_and_depth())
            .set_pipeline(pipeline)
            .draw(triangle);
    });

    return 0;
}
