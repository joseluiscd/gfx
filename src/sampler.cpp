#include <gfx/glad.h>
#include <gfx/log.hpp>
#include <gfx/sampler.hpp>

namespace gfx {

struct Sampler::Impl {
    GLuint id;

    Impl()
        : id(0)
    {
        glCreateSamplers(1, &id);
    }

    ~Impl()
    {
        glDeleteSamplers(1, &id);
    }
};

Sampler::Sampler()
    : impl(new Impl)
{
}

Sampler::~Sampler()
{
}

Sampler Sampler::default_sampler()
{
    static Sampler sampler;
    return sampler;
}

constexpr std::array<GLenum, SampleFilter::SampleFilter_COUNT> SampleFilter_GL = []() constexpr
{
    std::array<GLenum, SampleFilter_COUNT> ret {};

    ret[SampleFilter::Nearest] = GL_NEAREST;
    ret[SampleFilter::Linear] = GL_LINEAR;

    return ret;
}
();

Sampler& Sampler::min_filter(SampleFilter filter)
{
    GFX_ASSERT(0 <= filter && filter < SampleFilter_COUNT, "Invalide Sample filter");
    glSamplerParameteri(impl->id, GL_TEXTURE_MIN_FILTER, SampleFilter_GL[filter]);
    return *this;
}

Sampler& Sampler::mag_filter(SampleFilter filter)
{
    GFX_ASSERT(0 <= filter && filter < SampleFilter_COUNT, "Invalide Sample filter");
    glSamplerParameteri(impl->id, GL_TEXTURE_MAG_FILTER, SampleFilter_GL[filter]);
    return *this;
}

void Sampler::upload(TextureUnit unit)
{
    glBindSampler(unit, impl->id);
}

}