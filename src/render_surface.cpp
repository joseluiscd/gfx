#include <gfx/render_surface.hpp>

namespace gfx {

void RenderSurface::notify_resize_observers(const glm::ivec2& new_size)
{
    auto it = resize_observers.begin();
    for (; it != resize_observers.end(); ++it) {
        (*it)->resize_event(new_size);
    }
}

}