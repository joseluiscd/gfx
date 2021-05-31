#include <gfx/render_pipeline.hpp>
#include <spdlog/spdlog.h>

namespace gfx {


RenderPipeline RenderPipeline::Builder::build()
{
#ifdef GFX_VALIDATION
    if (shader.get() == nullptr) {
        spdlog::error("Creating pipeline {} without specifying shader program", label);
    }
#endif

    return RenderPipeline { label, shader, culling, blending };
}

void RenderPipeline::use()
{
    shader->use();
}

}