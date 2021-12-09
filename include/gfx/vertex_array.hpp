#pragma once

#include <algorithm>
#include <gfx/buffer.hpp>
#include <gfx/log.hpp>
#include <gfx/render_pipeline.hpp>
#include <initializer_list>
#include <memory>
#include <optional>
#include <vector>

#ifndef GFX_MAX_ATTRIBUTES
#define GFX_MAX_ATTRIBUTES 16
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

    template <typename T, typename = decltype(T::__gfx_layout)>
    VertexArray& add_buffer(const Buffer<T>& buffer);

    template <typename T, typename = decltype(T::__gfx_layout)>
    VertexArray& add_buffer(T* data, size_t count);

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

    void draw(); // override

private:
    std::vector<Layout> layouts;
    std::vector<RawBuffer> buffers; // Just for keeping the handle around until the VAO is destroyed
    std::optional<RawBuffer> elements;

    size_t element_count;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

enum Type : size_t {
    Byte,
    Float,
    Double,
    Int16,
    Int32,
    UnsignedByte,
    Unsigned16,
    Unsigned32,
};

constexpr const size_t Type_size[] = {
    sizeof(std::byte), // Byte
    sizeof(float), // Float
    sizeof(double), // Double
    sizeof(std::int16_t), // Int16
    sizeof(std::int32_t), // Int32
    sizeof(std::byte), // UnsignedByte
    sizeof(std::uint16_t), // Unsigned16
    sizeof(std::uint32_t), // Unsigned32
};

#define GFX_VERTEX_LAYOUT(...) static constexpr const ::gfx::VertexArray::Layout __gfx_layout = { __VA_ARGS__ };

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

    constexpr Layout(std::initializer_list<Entry> v)
        : entries()
        , size(0)
        , count(v.size())
    {
        GFX_ASSERT(v.size() <= GFX_MAX_ATTRIBUTES,
            "Number of attributes in a layout cannot exceed %d (%zu supplied)."
            "You can change that by defining GFX_MAX_ATTRIBUTES."
            "This number should not exceed the number of attributes that your GPU supports.",
            GFX_MAX_ATTRIBUTES, v.size());

        size_t i = 0;
        for (auto it = v.begin(); it != v.end(); ++it) {
            entries[i++] = *it;
            size += it->count * Type_size[it->type];
        }
    }

    size_t get_size() const { return size; }

private:
    std::array<Entry, 16> entries;
    size_t count;
    size_t size;

    void apply(const VertexArray& vao, const RawBuffer& buffer, unsigned index) const;
};

// TODO: Declare attribs in a DSL fashion
template <typename Type>
VertexArray::Layout::Entry attrib(int index)
{
}

template <typename T>
VertexArray& VertexArray::add_buffer(const Layout& layout, const Buffer<T>& buffer)
{
    GFX_ASSERT(sizeof(T) == layout.size, "Layout size must match array type size. Found %zu and %zu", sizeof(T), layout.size);
    element_count = buffer.size();
    return add_buffer(layout, buffer.get_raw());
}

template <typename T>
VertexArray& VertexArray::add_buffer(const Layout& layout, T* data, size_t count)
{
    GFX_ASSERT(sizeof(T) == layout.size, "Layout size must match array type size. Found %zu and %zu", sizeof(T), layout.size);
    element_count = count;
    return add_buffer(layout, RawBuffer(data, count * sizeof(T)));
}

template <typename T, typename>
inline VertexArray& VertexArray::add_buffer(const Buffer<T>& buffer)
{
    static_assert(std::is_same_v<std::remove_const_t<decltype(T::__gfx_layout)>, VertexArray::Layout>,
        "To use this method signature, the vertex type must define its layout");
    return add_buffer(T::__gfx_layout, buffer);
}

template <typename T, typename>
inline VertexArray& VertexArray::add_buffer(T* data, size_t count)
{
    static_assert(std::is_same_v<std::remove_const_t<decltype(T::__gfx_layout)>, VertexArray::Layout>,
        "To use this method signature, the vertex type must define its layout");
    return add_buffer(T::__gfx_layout, data, count);
}
}