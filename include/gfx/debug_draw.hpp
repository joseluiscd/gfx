#pragma once

#include <gfx/vertex_array.hpp>
#include <glm/glm.hpp>

namespace gfx {

struct DebugPoint {
    glm::vec3 point;
    glm::vec3 color;

    static const VertexArray::Layout layout;
};

class DebugDraw {
public:
    class Builder {
    public:
        Builder()
        {
            point_layers.push_back({0, 0});
            line_layers.push_back({0, 0});
            triangle_layers.push_back({0, 0});
        }

        void layer_next()
        {
            current_layer++;
            point_layers.push_back({points.size(), points.size()});
            line_layers.push_back({lines.size(), lines.size()});
            triangle_layers.push_back({triangles.size(), triangles.size()});
        }

        void set_color(const glm::vec3& color)
        {
            current_color = color;
        }
        void set_fill_color(const glm::vec3& color)
        {
            current_fill_color = color;
        }

        void enable_fill() { fill_polygons = true; }
        void disable_fill() { fill_polygons = false; }

        void point(const glm::vec3& p);
        void line(const glm::vec3& a, const glm::vec3& b);
        void triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

        void circle(const glm::vec3& center, const glm::vec3& normal, float radius);

        DebugDraw build();

    private:
        unsigned int current_layer = 0;
        bool fill_polygons = true;

        glm::vec3 current_color = { 0.0, 0.0, 0.0 };
        glm::vec3 current_fill_color = { 1.0, 0.0, 0.0 };

        std::vector<DebugPoint> geometry;

        std::vector<unsigned int> points;
        std::vector<unsigned int> lines;
        std::vector<unsigned int> triangles; // Strips

        // Ranges of indicies of the specified layer [start, end)
        // If start == end, then it is empty
        std::vector<glm::ivec2> point_layers;
        std::vector<glm::ivec2> line_layers;
        std::vector<glm::ivec2> triangle_layers;
    };

    static Builder init_file(const char* filename);

private:
    Buffer<DebugPoint> geometry;
    std::vector<Buffer<unsigned int>> indices;

    VertexArray points;
    VertexArray lines;
    VertexArray triangles;

    std::vector<glm::ivec2> point_layers;
    std::vector<glm::ivec2> line_layers;
    std::vector<glm::ivec2> triangle_layers;
};

}