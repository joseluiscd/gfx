#pragma once

#include <functional>
#include <gfx/render_surface.hpp>
#include <memory>

namespace gfx {

struct InitOptions {
    /// Window title
    const char* title = "GFX App";
    /// Fullscreen window
    bool fullscreen = false;

    /// Maximized;
    bool maximized = false;

    /// Window width and height
    int width = 0, height = 0;

    /// Enable window resize
    bool resizable = true;

    /// Enable ImGui
    bool imgui = false;

    /// Enable debug draw
    bool debug_draw = false;

    /// Enable debug context
    bool debug_context = false;
};

class WindowRenderSurface : public RenderSurface {
    friend class Gfx;

public:
    virtual glm::ivec2 size() override
    {
        return _size;
    }

    virtual void bind() override;

private:
    glm::ivec2 _size;
};

class Gfx {
    friend class WindowRenderSurface;

public:
    Gfx(const InitOptions& opts);
    ~Gfx();

    Gfx(Gfx&&) = delete;
    Gfx& operator=(Gfx&&) = delete;

    /// Gets the render surface of the window.
    /// Lifetime of the returned pointer is the same as the `Gfx` object.
    RenderSurface& get_surface()
    {
        return surface;
    }

    void render();
    bool should_close();
    void step(std::function<void()> render_frame);

    template <typename RenderFunc>
    void main_loop(RenderFunc&& render_frame)
    {
        while (!should_close()) {
            step(render_frame);
        }
    }

    /// Callback has signature `void callback(int key, int action, int mods)`
    using KeyboardCallback = std::function<void(int, int, int)>;
    /// Callback has signature `void callback(glm::ivec2 position)`
    using MouseMoveCallback = std::function<void(glm::vec2)>;
    /// Callback has signature `void callback(int button, int action, int mods)`
    using MouseButtonCallback = std::function<void(int, int, int)>;

    void set_keyboard_callback(KeyboardCallback callback);
    void set_mouse_move_callback(MouseMoveCallback callback);
    void set_mouse_button_callback(MouseButtonCallback callback);

    glm::vec2 get_mouse_position();

    void draw_gui(RenderSurface& rs);
    void draw_gui() { draw_gui(get_surface()); }

private:
    Gfx(const Gfx&) = delete;
    Gfx& operator=(const Gfx&) = delete;

    WindowRenderSurface surface;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace gfx