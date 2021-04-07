#include <gfx/debug_draw.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace gfx {

const VertexArray::Layout DebugPoint::layout = {
    { 1, 3, Type::Float },
    { 2, 3, Type::Float }
};

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

DebugDraw DebugDraw::Builder::build()
{
    DebugDraw dd;

    dd.geometry = Buffer<DebugPoint>(std::move(geometry));
    if (!points.empty()) {
        size_t i = dd.indices.size();
        dd.indices.emplace_back(std::move(points));

        dd.points
            .add_buffer(DebugPoint::layout, dd.geometry)
            .set_indices_buffer(dd.indices.back())
            .set_mode(Mode::Points);
    }

    if (!lines.empty()) {
        size_t i = dd.indices.size();
        dd.indices.emplace_back(std::move(lines));

        dd.lines
            .add_buffer(DebugPoint::layout, dd.geometry)
            .set_indices_buffer(dd.indices.back())
            .set_mode(Mode::Lines);
    }

    if (!triangles.empty()) {
        size_t i = dd.indices.size();
        dd.indices.emplace_back(std::move(triangles));

        dd.triangles
            .add_buffer(DebugPoint::layout, dd.geometry)
            .set_indices_buffer(dd.indices.back())
            .set_mode(Mode::Triangles);
    }

    return dd;
}

}

const char* point_line_shader_source_vertex = "\n"
                                              "layout (location=kPosition) in vec3 in_Position;\n"
                                              "layout (location=kColor) in vec3 in_Color;\n"
                                              "\n"
                                              "out vec4 v_Color;\n"
                                              "layout (location=kViewMatrix) uniform mat4 u_mvMatrix;\n"
                                              "layout (location=kProjectionMatrix) uniform mat4 u_pMatrix;\n"
                                              "\n"
                                              "void main()\n"
                                              "{\n"
                                              "    mat4 u_MvpMatrix = u_pMatrix * u_mvMatrix;"
                                              "    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);\n"
                                              "    v_Color      = vec4(in_Color, 1.0);\n"
                                              "}\n";

const char* point_line_shader_source_fragment = "\n"
                                                "in  vec4 v_Color;\n"
                                                "out vec4 out_FragColor;\n"
                                                "\n"
                                                "void main()\n"
                                                "{\n"
                                                "    out_FragColor = v_Color;\n"
                                                "}\n";