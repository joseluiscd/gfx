#pragma once

#include <gfx/sampler.hpp>
#include <gfx/texture.hpp>
#include <gfx/uniform.hpp>
#include <optional>

namespace gfx {

template <TextureUnit UNIT>
struct TextureSemantics : public gfx::UniformSemantics {
    using type = TextureUnit;
};

namespace texture::detail {
    void upload_texture(const TextureHandle& uniform, TextureUnit unit);
}

template <TextureUnit UNIT>
class TextureBinding : public Uniform<TextureSemantics<UNIT>> {
public:
    TextureBinding(const TextureHandle& h, const Sampler& s = Sampler::default_sampler())
        : Uniform<TextureSemantics<UNIT>>(UNIT)
        , texture(h)
        , sampler(s)
    {
    }

    void bind(ShaderBinder& binder)
    {
        texture::detail::upload_texture(texture, UNIT);
        sampler.upload(UNIT);

        // Uploads "TextureUnit" type to the shader
        Uniform<TextureSemantics<UNIT>>::bind(binder);
    }

private:
    TextureHandle texture;
    Sampler sampler;
};

}