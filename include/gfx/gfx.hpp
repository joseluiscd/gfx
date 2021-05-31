#pragma once

#include <functional>
#include <gfx/render_surface.hpp>
#include <memory>

namespace gfx {

struct InitOptions {
    /// Window title
    const char* title;
    /// Fullscreen window
    bool fullscreen;

    /// Maximized;
    bool maximized;

    /// Window width and height
    int width, height;

    /// Enable window resize
    bool resizable;

    /// Enable ImGui
    bool imgui;

    /// Enable debug draw
    bool debug_draw;

    /// Enable debug context
    bool debug_context;
};

class WindowRenderSurface : public RenderSurface {
    friend class Gfx;

public:
    virtual glm::ivec2 size()
    {
        return _size;
    }

    virtual void bind();

private:
    glm::ivec2 _size;
};

/// Callback has signature `void callback(int key, int action, int mods)`
using KeyboardCallback = std::function<void(int, int, int)>;
/// Callback has signature `void callback(glm::ivec2 position)`
using MouseMoveCallback = std::function<void(glm::vec2)>;
/// Callback has signature `void callback(int button, int action, int mods)`
using MouseButtonCallback = std::function<void(int, int, int)>;

class Gfx {
    friend class WindowRenderSurface;

public:
    Gfx(const InitOptions& opts);
    ~Gfx();

    /// Gets the render surface of the window.
    /// Lifetime of the returned pointer is the same as the `Gfx` object.
    RenderSurface& get_surface()
    {
        return surface;
    }

    void render();
    void main_loop(std::function<void()> render_frame);

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