#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace gfx {
    
using TextureUnit = unsigned int;

enum class TextureType {
    Rgb,
    Rgba,

    Bgr,
    Bgra,

    Depth
};

enum class TextureKind {
    Image,
    Multisample
};

size_t TextureType_get_size(TextureType t);

class TextureHandle {
public:
    TextureHandle();
    TextureHandle(TextureKind kind);
    TextureHandle(const TextureHandle& other);
    TextureHandle(TextureHandle&& other);
    ~TextureHandle();

    TextureHandle& operator=(const TextureHandle& other);
    TextureHandle& operator=(TextureHandle&& other);

    bool operator==(const TextureHandle& other);
    bool operator!=(const TextureHandle& other);

    size_t width() const;
    size_t height() const;
    glm::ivec2 size() const;

    void reserve_size(glm::ivec2 _size, unsigned mipmap_levels, TextureType type);
    void upload_data(void* data, unsigned level);
    void build_mipmaps();

    unsigned get_id() const;
    void bind() const;

    static TextureHandle get_empty();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
    TextureHandle(Impl* _impl);
};

class Texture : public TextureHandle {
public:
    Texture();
    Texture(const glm::ivec2& size, bool mipmaps = false);
    Texture(const glm::ivec2& size, size_t mipmaps);

private:
    size_t mipmaps;
};

}
