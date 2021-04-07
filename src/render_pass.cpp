#include <gfx/render_pass.hpp>
#include <gfx/glad.h>

namespace gfx {

RenderPass::RenderPass(RenderSurface& surface, ClearOperation clear)
{
    surface.bind();
    switch(clear.method) {
        case ClearOperation::kNothing:
        break;

        case ClearOperation::kColor:
        glClearColor(clear.clear_color->r, clear.clear_color->g, clear.clear_color->b, clear.clear_color->a);
        glClear(GL_COLOR_BUFFER_BIT);
        break;

        case ClearOperation::kColorDepth:
        glClearColor(clear.clear_color->r, clear.clear_color->g, clear.clear_color->b, clear.clear_color->a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        break;
        case ClearOperation::kDepth:
        glClear(GL_DEPTH_BUFFER_BIT);
        break;
    }
}

RenderPass& RenderPass::viewport(const glm::ivec2& origin, const glm::ivec2& size)
{
    glViewport(origin.x, origin.y, size.x, size.y);
    return *this;
}

}