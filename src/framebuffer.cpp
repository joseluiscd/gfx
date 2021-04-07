#include "private/pixel_storage.hpp"
#include <gfx/framebuffer.hpp>
#include <gfx/glad.h>
#include <spdlog/spdlog.h>

namespace gfx {

struct Framebuffer::Impl {
    GLuint id;
};

struct Framebuffer::Renderbuffer {
    Renderbuffer(const glm::ivec2& size, TextureType type, unsigned multisample = 0)
    {
        glCreateRenderbuffers(1, &id);
        if (multisample == 0) {
            glNamedRenderbufferStorage(id, TextureType_get_GL_internal(type), size.x, size.y);
        } else {
            glEnable(GL_MULTISAMPLE);
            glNamedRenderbufferStorageMultisample(id, multisample, TextureType_get_GL_internal(type), size.x, size.y);
        }
    }

    ~Renderbuffer()
    {
        glDeleteFramebuffers(1, &id);
    }

    GLuint id;
};

Framebuffer::Framebuffer()
    : impl(new Impl)
    , _size(0x7FFFFFFF, 0x7FFFFFFF)
    , tex_depth_buffer()
    , tex_color_buffers()
    , rb_depth_buffer(nullptr)
    , rb_color_buffers()
{
    glCreateFramebuffers(1, &impl->id);
}

Framebuffer::Framebuffer(Framebuffer&& other) = default;
Framebuffer& Framebuffer::operator=(Framebuffer&& other) = default;

Framebuffer::~Framebuffer()

{
    glDeleteFramebuffers(1, &impl->id);
}

Framebuffer& Framebuffer::add_color_buffer(TextureHandle texture)
{
    glNamedFramebufferTexture(
        impl->id,
        GL_COLOR_ATTACHMENT0 + rb_color_buffers.size() + tex_color_buffers.size(),
        texture.get_id(),
        0);

    tex_color_buffers.push_back(texture);
    _size = glm::min(texture.size(), _size);
    return *this;
}

Framebuffer& Framebuffer::add_color_buffer(const glm::ivec2& size, TextureType t, unsigned multisample)
{
    Renderbuffer* rb = new Renderbuffer(size, t, multisample);
    glNamedFramebufferRenderbuffer(
        impl->id,
        GL_COLOR_ATTACHMENT0 + rb_color_buffers.size() + tex_color_buffers.size(),
        GL_RENDERBUFFER,
        rb->id);

    rb_color_buffers.emplace_back(std::unique_ptr<Renderbuffer>(rb));
    _size = glm::min(size, _size);
    return *this;
}

Framebuffer& Framebuffer::set_depth_buffer(TextureHandle texture)
{
    glNamedFramebufferTexture(
        impl->id,
        GL_DEPTH_ATTACHMENT,
        texture.get_id(),
        0);

    tex_depth_buffer = texture;
    _size = glm::min(texture.size(), _size);

    return *this;
}

Framebuffer& Framebuffer::set_depth_buffer(const glm::ivec2& size, unsigned multisample)
{
    Renderbuffer* rb = new Renderbuffer(size, TextureType::Depth, multisample);
    glNamedFramebufferRenderbuffer(
        impl->id,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        rb->id);

    rb_depth_buffer.reset(rb);
    _size = glm::min(size, _size);
    return *this;
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, impl->id);
    glViewport(0, 0, _size.x, _size.y);

#ifdef GFX_VALIDATION
    static bool checked = false;
    if (!checked) {
        checked = true;
        if (glCheckNamedFramebufferStatus(impl->id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("Framebuffer is iscomplete");
        }
    }
#endif
}

void Framebuffer::blit_colors(
    const Framebuffer& dest,
    const glm::ivec2& source0,
    const glm::ivec2& source1,
    const glm::ivec2& dest0,
    const glm::ivec2& dest1)
{
    glBlitNamedFramebuffer(impl->id, dest.impl->id,
        source0.x, source0.y,
        source1.x, source1.y,
        dest0.x, dest0.y,
        dest1.x, dest0.y,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Framebuffer::blit_colors(
    const glm::ivec2& source0,
    const glm::ivec2& source1,
    const glm::ivec2& dest0,
    const glm::ivec2& dest1)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, impl->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        source0.x, source0.y,
        source1.x, source1.y,
        dest0.x, dest0.y,
        dest1.x, dest1.y,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

}