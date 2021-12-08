#include <gfx/render_pipeline.hpp>
#include <gfx/log.hpp>

namespace gfx {


RenderPipeline RenderPipeline::Builder::build()
{
    GFX_ASSERT(shader.get() != nullptr, "Creating pipeline %s without specifying shader program", label.c_str());
    return RenderPipeline { label, shader, culling, blending };
}

void RenderPipeline::use()
{
    shader->use();
}

}