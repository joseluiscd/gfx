#define _USE_MATH_DEFINES
#include <cmath>
#include <gfx/camera.hpp>
#include <gfx/debug_draw.hpp>
#include <gfx/glad.h>
#include <gfx/render_pass.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

const char* POINT_VS = R"(
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;

uniform mat4 mProj;
uniform mat4 mView;

out vec4 i_color;

void main()
{
    i_color = vec4(v_color, 1.0);
    gl_Position = mProj * mView * vec4(v_position, 1.0);
    gl_PointSize = 10.0;
}
)";

const char* POINT_FS = R"(
in vec4 i_color;

out vec4 f_color;

void main()
{
    f_color = i_color;
}
)";

namespace gfx {

class DebugDrawContext {
public:
    DebugDrawContext();

    RenderPipeline point_pipeline;
    RenderPipeline line_pipeline;
    RenderPipeline triangle_pipeline;
};

DebugDrawContext::DebugDrawContext()
    : point_pipeline(RenderPipeline::Builder("Point debug pipeline")
                         .with_shader(
                             ShaderProgram::Builder("Point debug shader")
                                 .register_class<CameraLens>("mProj")
                                 .register_class<CameraRig>("mView")
                                 .with_vertex_shader(POINT_VS)
                                 .with_fragment_shader(POINT_FS)
                                 .build())
                         .build())
    , line_pipeline(RenderPipeline::Builder("Line debug pipeline")
                        .with_shader(
                            ShaderProgram::Builder("Line debug shader")
                                .register_class<CameraLens>("mProj")
                                .register_class<CameraRig>("mView")
                                .with_vertex_shader(POINT_VS)
                                .with_fragment_shader(POINT_FS)
                                .build())
                        .build())
    , triangle_pipeline(RenderPipeline::Builder("Triangle debug pipeline")
                            .with_shader(
                                ShaderProgram::Builder("Triangle debug shader")
                                    .register_class<CameraLens>("mProj")
                                    .register_class<CameraRig>("mView")
                                    .with_vertex_shader(POINT_VS)
                                    .with_fragment_shader(POINT_FS)
                                    .build())
                            .build())
{
}

std::shared_ptr<DebugDrawContext> create_debug_draw_context()
{
    return std::make_shared<DebugDrawContext>();
}

void DebugDraw::Builder::point(const glm::vec3& p)
{
    size_t id = geometry.size();
    geometry.push_back(DebugPoint {
        p, current_color });

    points.push_back(id);
    point_layers[current_layer].y = points.size();
}

void DebugDraw::Builder::line(const glm::vec3& a, const glm::vec3& b)
{
    size_t id = geometry.size();
    geometry.push_back(DebugPoint { a, current_color });
    geometry.push_back(DebugPoint { b, current_color });

    lines.push_back(id);
    lines.push_back(id + 1);

    line_layers[current_layer].y = lines.size();
}

void DebugDraw::Builder::triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    size_t id = geometry.size();
    geometry.push_back(DebugPoint { a, current_color });
    geometry.push_back(DebugPoint { b, current_color });
    geometry.push_back(DebugPoint { c, current_color });

    triangles.push_back(id);
    triangles.push_back(id + 1);
    triangles.push_back(id + 2);

    triangle_layers[current_layer].y = triangles.size();
}

inline glm::vec3 random_perpendicular(glm::vec3 v)
{
    return v.z < v.x ? glm::vec3(v.y, -v.x, 0) : glm::vec3(0, -v.z, v.y);
}

void DebugDraw::Builder::circle(const glm::vec3& center, const glm::vec3& normal, float radius)
{
    const size_t CIRCLE_STEPS = 25;
    const float angle_step = M_PI * 2.0 / (CIRCLE_STEPS - 1);

    glm::vec3 n = glm::normalize(normal);
    glm::vec3 r = radius * glm::normalize(random_perpendicular(n));

    // Previous ID
    size_t id = geometry.size();
    geometry.push_back(DebugPoint { r, current_color });

    for (size_t i = 1; i < CIRCLE_STEPS; i++) {
        float angle = angle_step * i;
        glm::vec3 rot = glm::rotate(r, angle, n);

        geometry.push_back(DebugPoint { rot, current_color });
        lines.push_back(id);
        lines.push_back(++id);
    }

    line_layers[current_layer].y = lines.size();

    if (fill_polygons) {
        const size_t start = geometry.size();
        for (size_t i = 0; i < CIRCLE_STEPS; i++) {
            float angle = M_PI * 2.0 / (CIRCLE_STEPS - 1) * i;
            glm::vec3 rot = glm::rotate(r, angle, n);

            geometry.push_back(DebugPoint { rot, current_fill_color });
        }
        const size_t last = geometry.size();

        triangles.push_back(start);

        size_t i = start;
        size_t j = last;
        while (i < j) {
            triangles.push_back(++i);
            triangles.push_back(--j);
        }

        if (i == j) {
            triangles.push_back(i);
        }
        triangles.back() = 0xFFFFFFFF;
    }
}

DebugDraw DebugDraw::Builder::build(const std::shared_ptr<DebugDrawContext>& ctx)
{
    DebugDraw dd;
    dd.ctx = ctx;

    dd.geometry = Buffer<DebugPoint>(geometry);
    if (!points.empty()) {
        size_t i = dd.indices.size();
        dd.indices.emplace_back(points);

        dd.points
            .add_buffer(dd.geometry)
            .set_indices_buffer(dd.indices.back())
            .set_mode(Mode::Points);
    }

    if (!lines.empty()) {
        size_t i = dd.indices.size();
        dd.indices.emplace_back(lines);

        dd.lines
            .add_buffer(dd.geometry)
            .set_indices_buffer(dd.indices.back())
            .set_mode(Mode::Lines);
    }

    if (!triangles.empty()) {
        size_t i = dd.indices.size();
        dd.indices.emplace_back(triangles);

        dd.triangles
            .add_buffer(dd.geometry)
            .set_indices_buffer(dd.indices.back())
            .set_mode(Mode::Triangles);
    }

    return dd;
}


DebugDraw::DebugDraw()
    : ctx(nullptr)
    , geometry()
    , indices()
    , points()
    , lines()
    , triangles()
    , point_layers()
    , line_layers()
    , triangle_layers()
{
    points.set_mode(Mode::Points);
    lines.set_mode(Mode::Lines);
    triangles.set_mode(Mode::Triangles);
}

void DebugDraw::do_render_pass(RenderSurface& surface, gfx::CameraRig& cam)
{
    RenderPass(surface, ClearOperation::nothing())
        .viewport_full()
        .set_pipeline(ctx->point_pipeline)
        .with_camera(cam)
        .draw(points)
        .set_pipeline(ctx->line_pipeline)
        .with_camera(cam)
        .draw(lines)
        .set_pipeline(ctx->triangle_pipeline)
        .with_camera(cam)
        .draw(triangles)
        .end_pipeline();
}

}
