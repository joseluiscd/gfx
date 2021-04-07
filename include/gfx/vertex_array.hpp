#pragma once

#include <algorithm>
#include <gfx/buffer.hpp>
#include <gfx/render_pipeline.hpp>
#include <initializer_list>
#include <memory>
#include <optional>
#include <vector>

#ifdef GFX_VALIDATION
#include <spdlog/spdlog.h>
#endif

namespace gfx {

enum class Mode {
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan,
};

class VertexArray : public IDrawable<VertexArray> {
public:
    class Layout;

    VertexArray();
    VertexArray(const VertexArray& other) = delete;
    VertexArray& operator=(const VertexArray& other) = delete;
    VertexArray(VertexArray&& other);
    VertexArray& operator=(VertexArray&& other);

    virtual ~VertexArray();

    /// Add buffer contents to upload to the GPU.
    /// Size of T must be the same as the size of the layout.
    template <typename T>
    VertexArray& add_buffer(const Layout& layout, const Buffer<T>& buffer);
    template <typename T>
    VertexArray& add_buffer(const Layout& layout, T* data, size_t count);

    VertexArray& add_buffer(const Layout& layout, const RawBuffer& buffer);

    VertexArray& set_indices_buffer_16(const RawBuffer& data, size_t count);
    VertexArray& set_indices_buffer_32(const RawBuffer& data, size_t count);
    VertexArray& set_indices_buffer(uint16_t* data, size_t count);
    VertexArray& set_indices_buffer(uint32_t* data, size_t count);
    VertexArray& set_indices_buffer(const Buffer<uint16_t>& data);
    VertexArray& set_indices_buffer(const Buffer<uint32_t>& data);

    VertexArray& set_mode(Mode _mode);
    VertexArray& set_element_count(size_t count)
    {
        element_count = count;
        return *this;
    }

    void draw(); //override

private:
    std::vector<Layout> layouts;
    std::vector<RawBuffer> buffers; // Just for keeping the handle around until the VAO is destroyed
    std::optional<RawBuffer> elements;

    size_t element_count;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

enum class Type {
    Byte,
    Float,
    Double,
    Int16,
    Int32,
    UnsignedByte,
    Unsigned16,
    Unsigned32,
};

size_t Type_get_size(Type t);

/// Specifies layout of a Vertex Attribute Object
class VertexArray::Layout {
    friend class VertexArray;

public:
    /// Describe a vertex attribute
    struct Entry {
        /// Location to bind in the shader program. If negative, it is disabled (e.g. used as padding)
        int index;

        /// Number of components (float = 1, glm::vec3 = 3, etc.)
        unsigned int count;

        /// Type of argument (float = Float, glm::vec3 = Float, glm::ivec2 = Int)
        Type type;
    };

    Layout(std::initializer_list<Entry> v)
        : entries(v)
        , size(0)
    {
        for (auto it = v.begin(); it != v.end(); ++it) {
            size += it->count * Type_get_size(it->type);
        }
    }

    size_t get_size() const { return size; }

private:
    std::vector<Entry> entries;
    size_t size;

    void apply(const VertexArray& vao, const RawBuffer& buffer) const;
};

template <typename T>
VertexArray& VertexArray::add_buffer(const Layout& layout, const Buffer<T>& buffer)
{
#ifdef GFX_VALIDATION
    if (sizeof(T) != layout.size) {
        spdlog::error("Layout size must match array type size");
    }
#endif
    element_count = buffer.size();
    return add_buffer(layout, buffer.get_raw());
}

template <typename T>
VertexArray& VertexArray::add_buffer(const Layout& layout, T* data, size_t count)
{
#ifdef GFX_VALIDATION
    if (sizeof(T) != layout.size) {
        spdlog::error("Layout size must match array type size");
    }
#endif
    element_count = count;
    return add_buffer(layout, RawBuffer(data, count * sizeof(T)));
}

}