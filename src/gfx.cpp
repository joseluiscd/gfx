#include <algorithm>
#include <exception>
#include <gfx/font_awesome.hpp>
#include <gfx/gfx.hpp>
#include <gfx/glad.h>
#include <gfx/imgui/imgui.h>
#include <gfx/log.hpp>
#include <gfx/render_surface.hpp>
#include <iostream>
#include <optional>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#ifdef GFX_ENABLE_FONTAWESOME
#include "fonts/font_awesome.hpp"
#endif

#include "fonts/font_lato.hpp"

namespace gfx {

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);

void setup_imgui_style();

struct Gfx::Impl {
    GLFWwindow* window = nullptr;

    bool debug_draw_inited = false;
    bool imgui_inited = false;

    std::optional<KeyboardCallback> keyboard_cb = {};
    std::optional<MouseMoveCallback> mousemove_cb = {};
    std::optional<MouseButtonCallback> mousebutton_cb = {};

    static void resize_callback(GLFWwindow* w, int width, int height);
    static void mouse_move_callback(GLFWwindow* w, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* w, int button, int action, int mods);
    static void keyboard_callback(GLFWwindow* w, int key, int scancode, int action, int mods);
};

void Gfx::Impl::resize_callback(GLFWwindow* w, int width, int height)
{
    Gfx* gfx = (Gfx*)glfwGetWindowUserPointer(w);

    gfx->surface._size = glm::ivec2(width, height);

    gfx->surface.notify_resize_observers(gfx->surface._size);
}

void Gfx::Impl::mouse_move_callback(GLFWwindow* w, double xpos, double ypos)
{
    Gfx* gfx = (Gfx*)glfwGetWindowUserPointer(w);

    if (gfx->impl->mousemove_cb) {
        (*gfx->impl->mousemove_cb)(glm::vec2(xpos, ypos));
    }
}

void Gfx::Impl::mouse_button_callback(GLFWwindow* w, int button, int action, int mods)
{
    Gfx* gfx = (Gfx*)glfwGetWindowUserPointer(w);

    if (gfx->impl->mousebutton_cb) {
        (*gfx->impl->mousebutton_cb)(button, action, mods);
    }
}

void Gfx::Impl::keyboard_callback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    Gfx* gfx = (Gfx*)glfwGetWindowUserPointer(w);

    if (gfx->impl->keyboard_cb) {
        (*gfx->impl->keyboard_cb)(key, action, mods);
    }
}

void glfw_error_callback(int error, const char* description)
{
    std::printf("GLFW ERROR: %d, %s", error, description);
}

Gfx::Gfx(const InitOptions& opts)
    : impl(new Gfx::Impl)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        GFX_ERROR("Unable to open GLFW window");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, (int)opts.resizable);
    glfwWindowHint(GLFW_MAXIMIZED, (int)opts.maximized);

    if (opts.debug_context) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    }

    GLFWwindow* window = glfwCreateWindow(
        opts.width == 0 ? 640 : opts.width,
        opts.height == 0 ? 480 : opts.height,
        opts.title,
        nullptr,
        nullptr);

    GFX_ASSERT(window, "Unable to create window");

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    surface._size = glm::ivec2(width, height);

    impl->window = window;

    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // glfwSwapInterval(1);
    GFX_INFO("Open GL version: %s", glGetString(GL_VERSION));
    GFX_INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    GFX_INFO("Vendor: %s", glGetString(GL_VENDOR));
    GFX_INFO("Renderer: %s", glGetString(GL_RENDERER));

    glfwSetWindowSizeCallback(window, Impl::resize_callback);
    glfwSetMouseButtonCallback(window, Impl::mouse_button_callback);
    glfwSetKeyCallback(window, Impl::keyboard_callback);
    glfwSetCursorPosCallback(window, Impl::mouse_move_callback);

    if (opts.imgui) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOther(window, true);

        ImGui_ImplOpenGL3_Init();

        setup_imgui_style();

        impl->imgui_inited = true;
    }
    glEnable(GL_PROGRAM_POINT_SIZE);

    if (opts.debug_context) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

Gfx::~Gfx()
{
    if (impl) {
        if (impl->imgui_inited) {
            ImGui_ImplOpenGL3_Shutdown();
        }

        glfwDestroyWindow(impl->window);
        glfwTerminate();
    }
}

void Gfx::draw_gui(RenderSurface& rs)
{
    rs.bind();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Gfx::should_close()
{
    return glfwWindowShouldClose(impl->window);
}

void Gfx::step(std::function<void()> render_frame)
{
    glfwPollEvents();

    if (impl->imgui_inited) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    render_frame();

    glfwSwapBuffers(impl->window);
}

void WindowRenderSurface::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0.0, 0.0, _size.x, _size.y);
}

void Gfx::set_keyboard_callback(KeyboardCallback callback)
{
    this->impl->keyboard_cb = callback;
}

void Gfx::set_mouse_move_callback(MouseMoveCallback callback)
{
    this->impl->mousemove_cb = callback;
}

void Gfx::set_mouse_button_callback(MouseButtonCallback callback)
{
    this->impl->mousebutton_cb = callback;
}

glm::vec2 Gfx::get_mouse_position()
{
    double px, py;
    glfwGetCursorPos(this->impl->window, &px, &py);
    return glm::vec2(px, py);
}

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    }
    std::cout << std::endl;

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    }
    std::cout << std::endl;

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

#ifdef GFX_ENABLE_FONTAWESOME
void add_fontawesome_icons()
{
    ImFontConfig cfg;
    ImGuiIO& io = ImGui::GetIO();

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    cfg.MergeMode = true;
    cfg.PixelSnapH = true;
    cfg.GlyphMinAdvanceX = 20.0;
    cfg.GlyphMaxAdvanceX = 20.0;
    std::copy_n("FontAwesome", 12, cfg.Name);

    io.Fonts->AddFontFromMemoryCompressedBase85TTF(font_awesome_compressed_data_base85, 16.0, &cfg, icons_ranges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(font_awesome_2_compressed_data_base85, 16.0, &cfg, icons_ranges);
}
#else
#define add_fontawesome_icons()
#endif

void setup_imgui_style()
{
    ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.FramePadding = ImVec2(8.0, 3.0);

    ImGuiIO& io = ImGui::GetIO();

#ifdef GFX_ENABLE_LATO
    ImFontConfig cfg;
    std::copy_n("Lato", 5, cfg.Name);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(lato_compressed_data_base85, 16.0, &cfg);
    add_fontawesome_icons();
#endif

    io.Fonts->AddFontDefault();
    add_fontawesome_icons();
}

} // namespace gfx