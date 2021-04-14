#include <gfx/glad.h>
#include <gfx/vertex_array.hpp>

namespace gfx {

size_t Type_get_size(Type t)
{
    switch (t) {
    case Type::Byte:
        return 1;
    case Type::Float:
        return sizeof(float);
    case Type::Double:
        return sizeof(double);
    case Type::UnsignedByte:
        return 1;
    case Type::Int16:
        return sizeof(int16_t);
    case Type::Int32:
        return sizeof(int32_t);
    case Type::Unsigned16:
        return sizeof(uint16_t);
    case Type::Unsigned32:
        return sizeof(uint32_t);
    default:
        return 0;
    }
}

GLuint Type_get_GL(Type t)
{
    switch (t) {
    case Type::Byte:
        return GL_BYTE;
    case Type::Float:
        return GL_FLOAT;
    case Type::Double:
        return GL_DOUBLE;
    case Type::Int16:
        return GL_SHORT;
    case Type::Int32:
        return GL_INT;
    case Type::UnsignedByte:
        return GL_UNSIGNED_BYTE;
    case Type::Unsigned16:
        return GL_UNSIGNED_SHORT;
    case Type::Unsigned32:
        return GL_UNSIGNED_INT;
    default:
        return 0;
    }
}

struct VertexArray::Impl {
    GLuint id;
    GLuint elements_type;
    GLuint mode;

    ~Impl();
};

VertexArray::Impl::~Impl()
{
    glDeleteVertexArrays(1, &id);
}

VertexArray::VertexArray()
    : impl(new Impl)
    , layouts()
    , buffers()
    , elements()
    , element_count(0)
{
    glCreateVertexArrays(1, &impl->id);
}

VertexArray::VertexArray(VertexArray&& other) = default;

VertexArray& VertexArray::operator=(VertexArray&& other) = default;

VertexArray::~VertexArray()
{
}

void VertexArray::draw()
{
    glBindVertexArray(impl->id);

    if (elements) {
        glDrawElements(impl->mode, element_count, impl->elements_type, nullptr);
    } else {
        glDrawArrays(impl->mode, 0, element_count);
    }
}

VertexArray& VertexArray::add_buffer(const Layout& layout, const RawBuffer& buffer)
{

    buffers.push_back(buffer);
    layouts.push_back(layout);

    layout.apply(*this, buffer);

    return *this;
}

void VertexArray::Layout::apply(const VertexArray& vao, const RawBuffer& buffer) const
{
    size_t offset = 0;

    for (auto it = this->entries.begin(); it != this->entries.end(); ++it) {
        if (it->index >= 0) {
            glEnableVertexArrayAttrib(vao.impl->id, it->index);
            glVertexArrayAttribFormat(
                vao.impl->id,
                it->index,
                it->count,
                Type_get_GL(it->type),
                GL_FALSE,
                offset);

            glVertexArrayVertexBuffer(
                vao.impl->id,
                it->index,
                buffer.get_id(),
                offset,
                this->size);

            glVertexArrayAttribBinding(vao.impl->id, it->index, it->index);
        }

        offset += Type_get_size(it->type) * it->count;
    }
}

VertexArray& VertexArray::set_indices_buffer_32(const RawBuffer& data, size_t count)
{
    this->elements = std::make_optional(data);
    this->element_count = count;
    impl->elements_type = GL_UNSIGNED_INT;

    glVertexArrayElementBuffer(impl->id, data.get_id());

    return *this;
}

VertexArray& VertexArray::set_indices_buffer_16(const RawBuffer& data, size_t count)
{
    this->elements = std::make_optional(data);
    this->element_count = count;
    impl->elements_type = GL_UNSIGNED_SHORT;

    glVertexArrayElementBuffer(impl->id, data.get_id());

    return *this;
}

VertexArray& VertexArray::set_indices_buffer(uint16_t* data, size_t count)
{
    return set_indices_buffer_16(RawBuffer(data, count * sizeof(uint16_t)), count);
}

VertexArray& VertexArray::set_indices_buffer(uint32_t* data, size_t count)
{
    return set_indices_buffer_32(RawBuffer(data, count * sizeof(uint32_t)), count);
}

VertexArray& VertexArray::set_indices_buffer(const Buffer<uint16_t>& data)
{
    return set_indices_buffer_16(data.get_raw(), data.size());
}

VertexArray& VertexArray::set_indices_buffer(const Buffer<uint32_t>& data)
{
    return set_indices_buffer_32(data.get_raw(), data.size());
}

VertexArray& VertexArray::set_mode(Mode _mode)
{
    switch (_mode) {
    case Mode::Points:
        impl->mode = GL_POINTS;
        break;
    case Mode::Lines:
        impl->mode = GL_LINES;
        break;
    case Mode::LineStrip:
        impl->mode = GL_LINE_STRIP;
        break;
    case Mode::Triangles:
        impl->mode = GL_TRIANGLES;
        break;
    case Mode::TriangleStrip:
        impl->mode = GL_TRIANGLE_STRIP;
        break;
    case Mode::TriangleFan:
        impl->mode = GL_TRIANGLE_FAN;
        break;
    default:
        impl->mode = 0xFF;
        break;
    }

    return *this;
}

/*
VertexArray VertexArray::Builder::build()
{

#ifdef GFX_VALIDATION
    if (impl->id == 0) {
        spdlog::error("Vertex array object was not created");
    } 
    
    if (impl->vbos.size() == 0) {
        spdlog::warn("No buffers attached to the vertex array");
    }

    for (size_t i = 0; i < impl->vbos.size(); i++) {
        if (impl->vbos[i] == 0) {
            spdlog::error("The vbo nº {} was not created", i + 1);
        }
    }

    if (impl->mode == 0xFF) {
        spdlog::error("Must specify vertex array mode");
    }

#endif

    VertexArray vao;

    vao.layouts = std::move(layouts);
    
    vao.impl->id = impl->id;
    vao.impl->vbos = std::move(impl->vbos);
    vao.impl->mode = impl->mode;
    vao.impl->elements_vbo = impl->elements_vbo;
    vao.impl->elements_type = impl->elements_type;
    vao.impl->elements_count = impl->elements_count;
    vao.impl->arrays_lenght = impl->arrays_lenght;
    
    return vao;
}
*/

}