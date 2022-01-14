#include <gfx/camera.hpp>
#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pass.hpp>
#include <gfx/render_pipeline.hpp>
#include <gfx/texture_binding.hpp>
#include <gfx/uniform.hpp>
#include <gfx/vertex_array.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Define all attributes used by shaders
enum Attributes : unsigned int {
    Position = 0,
    Normal = 1,
    TexCoord = 2,
};

// Define all texture units used by shaders
enum TextureUnits : gfx::TextureUnit {
    Color = 0
};

using ColorTexture = gfx::TextureBinding<TextureUnits::Color>;

// Define our vertex struct with its layout used by VAOs
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;

    // Layout
    GFX_VERTEX_LAYOUT(
        gfx::attrib<glm::vec3>(Attributes::Position),
        gfx::attrib<glm::vec3>(Attributes::Normal),
        gfx::attrib<glm::vec2>(Attributes::TexCoord));
};

// Vertex shader source
const char* VS = R"(
layout (location = kPosition) in vec3 v_position;
layout (location = kNormal) in vec3 v_normal;
layout (location = kTexCoord) in vec2 v_texcoord;

uniform mat4 mProj;
uniform mat4 mModel;
uniform mat4 mView;

out vec4 i_position;
out vec4 i_normal;
out vec2 i_texcoord;

void main()
{
    i_normal = mView * mModel * vec4(v_normal, 0.0);
    i_texcoord = v_texcoord;
    i_position = mView * mModel * vec4(v_position, 1.0);
    gl_Position = mProj * mView * mModel * vec4(v_position, 1.0);
}
)";

const char* FS = R"(
in vec4 i_position;
in vec4 i_normal;
in vec2 i_texcoord;

uniform sampler2D color;

out vec4 f_color;

void main()
{
    vec4 color = texture(color, i_texcoord);
    f_color = max(dot(vec3(0.0, 0.0, 1.0), normalize(vec3(i_normal))), 0.0) * color;
}
)";

gfx::TextureHandle load_texture()
{
    gfx::TextureHandle texture;
    texture.reserve_size({ 4, 4 }, 1, gfx::TextureFormat::Rgba);
    uint8_t texture_data[4 * 4 * 4] = {
        0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, //
        0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, //
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, //
        0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, //
    };

    texture.upload_data(texture_data, 0);

    return texture;
}

gfx::Sampler create_sampler()
{
    gfx::Sampler sampler;
    sampler
        .min_filter(gfx::SampleFilter::Nearest)
        .mag_filter(gfx::SampleFilter::Nearest);
    return sampler;
}

class Object : public gfx::IBindable<Object>, public gfx::IDrawable<Object> {
public:
    struct ModelMatrix : public gfx::UniformSemantics {
        using type = glm::mat4;
    };

    Object()
        : modelView()
        , vao()
        , texture(load_texture(), create_sampler())
    {

        // Copied from wgpu examples
        gfx::Buffer<Vertex> vertices({
            // top (0, 0, 1)
            { { -1, -1, 1 }, { 0, 0, 1 }, { 0, 0 } },
            { { 1, -1, 1 }, { 0, 0, 1 }, { 1, 0 } },
            { { 1, 1, 1 }, { 0, 0, 1 }, { 1, 1 } },
            { { -1, 1, 1 }, { 0, 0, 1 }, { 0, 1 } },
            // bottom (0, 0, -1)
            { { -1, 1, -1 }, { 0, 0, -1 }, { 1, 0 } },
            { { 1, 1, -1 }, { 0, 0, -1 }, { 0, 0 } },
            { { 1, -1, -1 }, { 0, 0, -1 }, { 0, 1 } },
            { { -1, -1, -1 }, { 0, 0, -1 }, { 1, 1 } },
            // right (1, 0, 0)
            { { 1, -1, -1 }, { 1, 0, 0 }, { 0, 0 } },
            { { 1, 1, -1 }, { 1, 0, 0 }, { 1, 0 } },
            { { 1, 1, 1 }, { 1, 0, 0 }, { 1, 1 } },
            { { 1, -1, 1 }, { 1, 0, 0 }, { 0, 1 } },
            // left (-1, 0, 0)
            { { -1, -1, 1 }, { -1, 0, 0 }, { 1, 0 } },
            { { -1, 1, 1 }, { -1, 0, 0 }, { 0, 0 } },
            { { -1, 1, -1 }, { -1, 0, 0 }, { 0, 1 } },
            { { -1, -1, -1 }, { -1, 0, 0 }, { 1, 1 } },
            // front (0, 1, 0)
            { { 1, 1, -1 }, { 0, 1, 0 }, { 1, 0 } },
            { { -1, 1, -1 }, { 0, 1, 0 }, { 0, 0 } },
            { { -1, 1, 1 }, { 0, 1, 0 }, { 0, 1 } },
            { { 1, 1, 1 }, { 0, 1, 0 }, { 1, 1 } },
            // back (0, -1, 0)
            { { 1, -1, 1 }, { 0, -1, 0 }, { 0, 0 } },
            { { -1, -1, 1 }, { 0, -1, 0 }, { 1, 0 } },
            { { -1, -1, -1 }, { 0, -1, 0 }, { 1, 1 } },
            { { 1, -1, -1 }, { 0, -1, 0 }, { 0, 1 } },
        });

        gfx::Buffer<uint16_t> indices({
            0, 1, 2, 2, 3, 0, // top
            4, 5, 6, 6, 7, 4, // bottom
            8, 9, 10, 10, 11, 8, // right
            12, 13, 14, 14, 15, 12, // left
            16, 17, 18, 18, 19, 16, // front
            20, 21, 22, 22, 23, 20, // back
        });

        vao
            .add_buffer(vertices)
            .set_indices_buffer(indices)
            .set_mode(gfx::Mode::Triangles);

        *this->modelView = glm::translate(glm::vec3(0.0f, 0.0f, -2.0f));
    }

    virtual void bind(gfx::ShaderBinder& binder)
    {
        modelView.bind(binder);
        texture.bind(binder);
    }

    virtual void draw()
    {
        vao.draw();
    }

    void rotate()
    {
        *modelView = (*modelView) * glm::rotate(0.01f, glm::vec3(0.0f, 1.0f, 1.0f));
    }

private:
    gfx::Uniform<ModelMatrix> modelView;
    ColorTexture texture;
    gfx::VertexArray vao;
};

int main()
{
    gfx::InitOptions options;
    options.title = "Hello world";
    options.resizable = false;

    gfx::Gfx app(options);

    gfx::RenderPipeline pipeline = gfx::RenderPipeline::Builder("example")
                                       .with_shader(gfx::ShaderProgram::Builder("example")
                                                        .register_class<gfx::CameraLens>("mProj")
                                                        .register_class<gfx::CameraRig>("mView")
                                                        .register_uniform<Object::ModelMatrix>("mModel")
                                                        .register_texture<TextureUnits::Color>("color")
                                                        .with_constant("kPosition", Attributes::Position)
                                                        .with_constant("kNormal", Attributes::Normal)
                                                        .with_constant("kTexCoord", Attributes::TexCoord)
                                                        .with_vertex_shader(VS)
                                                        .with_fragment_shader(FS)
                                                        .build())
                                       .build();

    gfx::PerspectiveCameraLens lens(70.0, 1.0, 0.2, 10.2);
    app.get_surface().add_resize_observer(lens);

    gfx::CameraRig cam = gfx::CameraRig(lens)
                             .with_position({ 0.0, 0.0, 5.0 })
                             .look_at({ 0.0, 0.0, 0.0 })
                             .with_up_vector({ 0.0, 1.0, 0.0 });

    Object obj;
    glEnable(GL_DEPTH_TEST);

    app.main_loop([&]() {
        glClear(GL_DEPTH_BUFFER_BIT);

        gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color_and_depth({ 0.3, 0.3, 0.3, 1.0 }))
            .set_pipeline(pipeline)
            .with_camera(cam)
            .bind(obj)
            .draw(obj);

        obj.rotate();
    });

    return 0;
}
