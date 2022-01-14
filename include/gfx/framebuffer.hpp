#pragma once

#include <gfx/render_surface.hpp>
#include <gfx/texture.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace gfx {

class Framebuffer : public RenderSurface {
public:
    struct Renderbuffer;

    Framebuffer();
    Framebuffer(Framebuffer&& other);
    Framebuffer& operator=(Framebuffer&& other); 

    virtual ~Framebuffer();

    virtual glm::ivec2 size() override { return _size; }
    virtual void bind() override;

    Framebuffer& add_color_buffer(TextureHandle texture);
    Framebuffer& add_color_buffer(const glm::ivec2& size, TextureFormat t = TextureFormat::Rgba, unsigned multisample = 0);
    Framebuffer& set_depth_buffer(TextureHandle texture);
    Framebuffer& set_depth_buffer(const glm::ivec2& size, unsigned multisample = 0);

    void blit_colors(
        const Framebuffer& dest,
        const glm::ivec2& source0,
        const glm::ivec2& source1,
        const glm::ivec2& dest0,
        const glm::ivec2& dest1);

    void blit_colors(
        const glm::ivec2& source0,
        const glm::ivec2& source1,
        const glm::ivec2& dest0,
        const glm::ivec2& dest1);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    glm::ivec2 _size;

    std::vector<TextureHandle> tex_color_buffers;
    std::vector<std::unique_ptr<Renderbuffer>> rb_color_buffers;

    std::optional<TextureHandle> tex_depth_buffer;
    std::unique_ptr<Renderbuffer> rb_depth_buffer;
};

}