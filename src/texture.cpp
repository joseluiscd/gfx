#include <gfx/glad.h>
#include <gfx/texture.hpp>

namespace gfx {

struct TextureHandle::Impl {
    ~Impl();

    GLuint id = 0;
    GLenum kind = GL_TEXTURE_2D;
    glm::ivec2 _size = { 0, 0 };
    TextureType tex_type = TextureType::Rgb;
};

TextureHandle::Impl::~Impl()
{
    glDeleteTextures(1, &id);
}

GLenum TextureType_get_GL_internal(TextureType t)
{
    switch (t) {
    case TextureType::Rgb:
        return GL_RGB8;
    case TextureType::Rgba:
        return GL_RGBA8;
    case TextureType::Depth:
        return GL_DEPTH_COMPONENT32F;
    case TextureType::Bgr:
        return GL_RGB8;
    case TextureType::Bgra:
        return GL_RGBA8;
    default:
        return 0;
    }
}

GLenum TextureType_get_GL(TextureType t)
{
    switch (t) {
    case TextureType::Rgb:
        return GL_RGB;
    case TextureType::Rgba:
        return GL_RGBA;
    case TextureType::Depth:
        return GL_DEPTH_COMPONENT;
    case TextureType::Bgr:
        return GL_BGR;
    case TextureType::Bgra:
        return GL_BGRA;
    default:
        return 0;
    }
}

GLenum TextureType_get_GL_type(TextureType t)
{
    switch (t) {
    case TextureType::Rgb:
    case TextureType::Rgba:
    case TextureType::Bgr:
    case TextureType::Bgra:
        return GL_UNSIGNED_BYTE;
    case TextureType::Depth:
        return GL_FLOAT;
    default:
        return 0;
    }
}

GLenum TextureKind_get_GL(TextureKind k)
{
    switch (k) {
    case TextureKind::Image:
        return GL_TEXTURE_2D;
    case TextureKind::Multisample:
        return GL_TEXTURE_2D_MULTISAMPLE;
    default:
        return 0;
    }
}

TextureHandle::TextureHandle()
    : impl(new Impl)
{
    impl->kind = GL_TEXTURE_2D;
    glCreateTextures(GL_TEXTURE_2D, 1, &impl->id);
}

TextureHandle::TextureHandle(TextureKind k)
    : impl(new Impl)
{
    impl->kind = TextureKind_get_GL(k);
    glCreateTextures(impl->kind, 1, &impl->id);
}

TextureHandle::TextureHandle(Impl* _impl)
    : impl(_impl)
{
}

TextureHandle::~TextureHandle()
{
}

TextureHandle::TextureHandle(const TextureHandle& other) = default;
TextureHandle::TextureHandle(TextureHandle&& other) = default;

TextureHandle& TextureHandle::operator=(const TextureHandle& other) = default;
TextureHandle& TextureHandle::operator=(TextureHandle&& other) = default;

bool TextureHandle::operator==(const TextureHandle& other)
{
    return impl.get() == other.impl.get();
}

bool TextureHandle::operator!=(const TextureHandle& other)
{
    return impl.get() != other.impl.get();
}

void TextureHandle::reserve_size(glm::ivec2 _size, unsigned levels, TextureType t)
{
    impl->_size = _size;
    impl->tex_type = t;

    switch (impl->kind) {
    case GL_TEXTURE_2D:
        glTextureStorage2D(impl->id, levels, TextureType_get_GL_internal(t), _size.x, _size.y);
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        glTextureStorage2DMultisample(impl->id, levels, TextureType_get_GL_internal(t), _size.x, _size.y, true);
        break;
    default:
        break;
    }
}

void TextureHandle::upload_data(void* data, unsigned level)
{
    glTextureSubImage2D(
        impl->id, level,
        0, 0, impl->_size.x, impl->_size.y,
        TextureType_get_GL(impl->tex_type),
        TextureType_get_GL_type(impl->tex_type),
        data);
}

void TextureHandle::build_mipmaps()
{
    glGenerateTextureMipmap(impl->id);
}

glm::ivec2 TextureHandle::size() const
{
    return impl->_size;
}

unsigned TextureHandle::get_id() const
{
    return impl->id;
}

void TextureHandle::bind() const
{
    glBindTexture(impl->kind, impl->id);
}

TextureHandle TextureHandle::get_empty()
{
    static TextureHandle empty = TextureHandle(new TextureHandle::Impl());
    return empty;
}

}