#include <gfx/buffer.hpp>
#include <gfx/glad.h>

namespace gfx {

struct BufferHandle::Impl {
    ~Impl();

    GLuint id;
    size_t size;
    const void* buffer;
};

BufferHandle::BufferHandle()
    : impl(new Impl)
{
    impl->size = 0;
    impl->buffer = nullptr;

    glCreateBuffers(1, &impl->id);
}

BufferHandle::~BufferHandle()
{
}

BufferHandle::Impl::~Impl()
{
    glDeleteBuffers(1, &id);
}

void BufferHandle::update_raw_buffer(bool dynamic)
{
    glNamedBufferData(impl->id, impl->size, impl->buffer, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void BufferHandle::update_raw_buffer_bytes(const void* buffer, size_t size, bool dynamic)
{
    impl->buffer = buffer;
    impl->size = size;
    this->update_raw_buffer(dynamic);
}

void BufferHandle::bind_vertex_array() const
{
    glBindBuffer(GL_ARRAY_BUFFER, impl->id);
}

void BufferHandle::bind_element_array() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, impl->id);
}

unsigned int BufferHandle::get_id() const {
    return impl->id;
}

}