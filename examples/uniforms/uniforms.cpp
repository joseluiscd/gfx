#include <gfx/camera.hpp>
#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pass.hpp>
#include <gfx/render_pipeline.hpp>
#include <gfx/uniform.hpp>
#include <gfx/vertex_array.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

enum Attrib : unsigned int {
    Position = 0,
    Normal = 1,
    Color = 2,
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    GFX_VERTEX_LAYOUT(
        gfx::attrib<glm::vec3>(Attrib::Position),
        gfx::attrib<glm::vec3>(Attrib::Normal),
        gfx::attrib<glm::vec3>(Attrib::Color));
};

const char* VS = R"(
layout (location = kPosition) in vec3 v_position;
layout (location = kNormal) in vec3 v_normal;
layout (location = kColor) in vec3 v_color;


uniform mat4 mProj;
uniform mat4 mModel;
uniform mat4 mView;

out vec4 i_color;
out vec4 i_normal;
out vec4 i_position;

void main()
{
    i_normal = mView * mModel * vec4(v_normal, 0.0);
    i_color = vec4(v_color, 1.0);
    i_position = mView * mModel * vec4(v_position, 1.0);
    gl_Position = mProj * mView * mModel * vec4(v_position, 1.0);
}
)";

const char* FS = R"(
in vec4 i_color;
in vec4 i_normal;
in vec4 i_position;

out vec4 f_color;

void main()
{
    f_color = max(dot(vec3(0.0, 0.0, 1.0), normalize(vec3(i_normal))), 0.0) * i_color;
}
)";

class Object : public gfx::IBindable<Object>, public gfx::IDrawable<Object> {
public:
    struct ModelMatrix : public gfx::UniformSemantics {
        using type = glm::mat4;
    };

    Object()
        : modelView()
        , vao()
    {
        gfx::Buffer<Vertex> vertices(
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

        gfx::Buffer<uint16_t> indices({
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

        vao
            .add_buffer(vertices)
            .set_indices_buffer(indices)
            .set_mode(gfx::Mode::Triangles);

        *this->modelView = glm::translate(glm::vec3(0.0f, 0.0f, -2.0f));
    }

    virtual void bind(gfx::ShaderBinder& binder)
    {
        modelView.bind(binder);
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
                                                        .with_constant("kPosition", Attrib::Position)
                                                        .with_constant("kNormal", Attrib::Normal)
                                                        .with_constant("kColor", Attrib::Color)
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
