#include <gfx/buffer.hpp>
#include <gfx/glad.h>
#include <spdlog/spdlog.h>

namespace gfx {

struct RawBuffer::Impl {
    ~Impl();

    GLuint id;
    size_t size;
    const void* buffer;
};

RawBuffer::RawBuffer()
    : impl(new Impl)
{
    impl->size = 0;
    impl->buffer = nullptr;

    glCreateBuffers(1, &impl->id);
}

RawBuffer::~RawBuffer()
{
}

RawBuffer::Impl::~Impl()
{
    glDeleteBuffers(1, &id);
}

void RawBuffer::update_raw_buffer(bool dynamic)
{
    glNamedBufferData(impl->id, impl->size, impl->buffer, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void RawBuffer::update_raw_buffer_bytes(const void* buffer, size_t size, bool dynamic)
{
    impl->buffer = buffer;
    impl->size = size;
    this->update_raw_buffer(dynamic);
}

void RawBuffer::bind_vertex_array() const
{
    glBindBuffer(GL_ARRAY_BUFFER, impl->id);
}

void RawBuffer::bind_element_array() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, impl->id);
}

unsigned int RawBuffer::get_id() const {
    return impl->id;
}

}