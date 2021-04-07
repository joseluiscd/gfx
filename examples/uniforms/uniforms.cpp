#define GFX_VALIDATION

#include <gfx/camera.hpp>
#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/render_pipeline.hpp>
#include <gfx/uniform.hpp>
#include <gfx/vertex_array.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <spdlog/spdlog.h>
#include <gfx/render_pass.hpp>

enum Attrib : unsigned int {
    Position = 0,
    Normal = 1,
    Color = 2,
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static const gfx::VertexArray::Layout layout;
};

const gfx::VertexArray::Layout Vertex::layout = {
    { Attrib::Position, 3, gfx::Type::Float },
    { Attrib::Normal, 3, gfx::Type::Float },
    { Attrib::Color, 3, gfx::Type::Float },
};

class Object : public gfx::IBindable<Object>, public gfx::IDrawable<Object> {
public:
    struct ModelMatrix : public gfx::UniformSemantics {
        using type = glm::mat4;
        static constexpr const char* location = "kModelMatrix";
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
            .add_buffer(Vertex::layout, std::move(vertices))
            .set_indices_buffer(std::move(indices))
            .set_mode(gfx::Mode::Triangles);

        *this->modelView = glm::translate(glm::vec3(0.0f, 0.0f, -2.0f));
    }

    virtual void bind()
    {
        modelView.bind();
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
    gfx::Gfx app(gfx::InitOptions {
        .title = "Hello world",
        .resizable = true });

    spdlog::info("{}  - {}", Vertex::layout.get_size(), sizeof(Vertex));

    gfx::RenderPipeline pipeline = gfx::RenderPipeline::Builder("example")
                                       .with_shader(gfx::ShaderProgram::Builder("example")
                                                        .register_uniform<Object::ModelMatrix>()
                                                        .register_class<gfx::CameraLens>()
                                                        .register_class<gfx::CameraRig>()
                                                        .with_constant("kPosition", Attrib::Position)
                                                        .with_constant("kNormal", Attrib::Normal)
                                                        .with_constant("kColor", Attrib::Color)
                                                        .with_vertex_shader_file("example.vs")
                                                        .with_fragment_shader_file("example.fs")
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

        gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color_and_depth({0.3, 0.3, 0.3, 1.0}))
            .set_pipeline(pipeline)
            .with_camera(cam)
            .bind(obj)
            .draw(obj);

        obj.rotate();
    });

    return 0;
}
