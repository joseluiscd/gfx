#pragma once

#include <gfx/texture.hpp>

namespace gfx {

enum SampleFilter {
    Nearest,
    Linear,

    SampleFilter_COUNT
};

class Sampler {
public:
    Sampler();
    ~Sampler();

    void upload(TextureUnit unit);

    Sampler& min_filter(SampleFilter filter);
    Sampler& mag_filter(SampleFilter filter);

    static Sampler default_sampler();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

}