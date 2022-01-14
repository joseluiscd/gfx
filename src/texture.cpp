#include <gfx/glad.h>
#include <gfx/texture.hpp>
#include <gfx/log.hpp>

namespace gfx {

struct TextureHandle::Impl {
    ~Impl();

    GLuint id = 0;
    GLenum kind = GL_TEXTURE_2D;
    glm::ivec2 _size = { 0, 0 };
    TextureFormat tex_format = TextureFormat::Rgb;
};

TextureHandle::Impl::~Impl()
{
    glDeleteTextures(1, &id);
}

GLenum TextureFormat_get_GL_internal(TextureFormat t)
{
    switch (t) {
    case TextureFormat::Rgb:
        return GL_RGB8;
    case TextureFormat::Rgba:
        return GL_RGBA8;
    case TextureFormat::Depth:
        return GL_DEPTH_COMPONENT32F;
    case TextureFormat::Bgr:
        return GL_RGB8;
    case TextureFormat::Bgra:
        return GL_RGBA8;
    default:
        return 0;
    }
}

GLenum TextureFormat_get_GL(TextureFormat t)
{
    switch (t) {
    case TextureFormat::Rgb:
        return GL_RGB;
    case TextureFormat::Rgba:
        return GL_RGBA;
    case TextureFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case TextureFormat::Bgr:
        return GL_BGR;
    case TextureFormat::Bgra:
        return GL_BGRA;
    default:
        return 0;
    }
}

GLenum TextureFormat_get_GL_type(TextureFormat t)
{
    switch (t) {
    case TextureFormat::Rgb:
    case TextureFormat::Rgba:
    case TextureFormat::Bgr:
    case TextureFormat::Bgra:
        return GL_UNSIGNED_BYTE;
    case TextureFormat::Depth:
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

void TextureHandle::reserve_size(glm::ivec2 _size, unsigned levels, TextureFormat t)
{
    impl->_size = _size;
    impl->tex_format = t;

    switch (impl->kind) {
    case GL_TEXTURE_2D:
        glTextureStorage2D(impl->id, levels, TextureFormat_get_GL_internal(t), _size.x, _size.y);
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        glTextureStorage2DMultisample(impl->id, levels, TextureFormat_get_GL_internal(t), _size.x, _size.y, true);
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
        TextureFormat_get_GL(impl->tex_format),
        TextureFormat_get_GL_type(impl->tex_format),
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

std::vector<uint8_t> TextureHandle::read_colors()
{
    GFX_ASSERT(impl->tex_format != TextureFormat::Depth);

    size_t count = impl->_size.x * impl->_size.y;
    switch (impl->tex_format) {
    case TextureFormat::Bgr:
    case TextureFormat::Rgb:
        count *= 3;
        break;
    case TextureFormat::Bgra:
    case TextureFormat::Rgba:
        count *= 4;
        break;
    }
       
    std::vector<uint8_t> colors(count);
    glGetTextureImage(impl->id, 0, TextureFormat_get_GL(impl->tex_format), GL_UNSIGNED_BYTE, colors.size(), colors.data());
    return colors;
}

}