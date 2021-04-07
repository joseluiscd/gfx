#pragma once

#include <glm/glm.hpp>
#include <list>

namespace gfx {

class IResizeObserver {
public:
    virtual void resize_event(const glm::ivec2& new_size) = 0;
};

class RenderSurface {
public:
    RenderSurface()
        : resize_observers()
    {
    }

    virtual glm::ivec2 size() = 0;
    virtual void bind() = 0;

    void add_resize_observer(IResizeObserver& observer)
    {
        resize_observers.push_back(&observer);
        observer.resize_event(this->size());
    }

    void remove_resize_observer(IResizeObserver& observer)
    {
        resize_observers.remove(&observer);
    }

protected:
    void notify_resize_observers(const glm::ivec2& new_size);

    std::list<IResizeObserver*> resize_observers;
};

}
