#pragma once

#include <gfx/bindable.hpp>
#include <gfx/render_pipeline.hpp>
#include <gfx/render_surface.hpp>
#include <optional>

namespace gfx {

class RenderPass;

/**
* Framebuffer Clear operation.
*/
class ClearOperation {
public:
    friend class RenderPass;
    enum {
        kNothing = 0,
        kColor,
        kDepth,
        kColorDepth,
    };

    /// Clear operation that cleans nothing.
    static ClearOperation nothing() { return ClearOperation { kNothing, {} }; }

    /// Clear operation that only cleans the color buffer.
    /// @param color Color to clear the buffer with
    static ClearOperation color(const glm::vec4& color = { 0.0, 0.0, 0.0, 0.0 })
    {
        return ClearOperation(kColor, color);
    }

    /// Clear operation that clears the color buffer and the depth buffer.
    /// @param color Color to clear the buffer with
    static ClearOperation color_and_depth(const glm::vec4& color = { 0.0, 0.0, 0.0, 0.0 })
    {
        return ClearOperation(kColorDepth, color);
    }

private:
    int method;
    std::optional<glm::vec4> clear_color;

    ClearOperation(int _method, std::optional<glm::vec4> _color)
        : method(_method)
        , clear_color(_color)
    {
    }
};

/**
* @brief This class encapsulates a render pass.
*
* All methods return the current instance of the class as a convenience for chaining method calls.
* All render operations must be performed by a render pass.
* @note It is not needed to store it anywhere: for each render pass, create a new object.
*/
class RenderPass {
public:
    /** 
    * @brief Create a RenderPass.
    * @param surface Target RenderSurface to draw to.
    * @param clear ClearOperation to apply before any draw calls.
    */
    RenderPass(RenderSurface& surface, ClearOperation clear = ClearOperation::color_and_depth());
    ~RenderPass() { }

    // Delete constructors and assign operators.
    RenderPass(const RenderPass& other) = delete;
    RenderPass& operator=(const RenderPass& other) = delete;
    RenderPass(RenderPass&& other) = delete;
    RenderPass& operator=(RenderPass&& other) = delete;

    /// @brief Object to encapsulate RenderPass operations when a RenderPipeline is in use.
    class Pipeline {
    public:
        friend class RenderPass;

        /**
        * @brief Switches the pipeline to the new one without destroying the object.
        *
        * Use this method to avoid using `Pipeline::end_pipeline()` and calling again `RenderPass::set_pipeline()`.
        * @param p The new RenderPipeline.
        */
        [[nodiscard]] Pipeline& set_pipeline(RenderPipeline& p)
        {
            this->pipeline = &p;
            this->pipeline->use();
            return *this;
        }
        
        /**
        * @brief Stop using this pipeline and continue using the underlying RenderPass object.
        * @note There is no need to call this method at the end of the render pass.
        */
        RenderPass& end_pipeline()
        {
            return *this->pass;
        }

        /// Bind camera to the pipeline. Same as RenderPass::bind.
        template <typename Camera>
        [[nodiscard]] RenderPass::Pipeline& with_camera(ICamera<Camera>& c)
        {
            c.bind(pipeline->get_binder());
            return *this;
        }

        /// Bind some value to the pipeline
        template <typename Bindable>
        [[nodiscard]] RenderPass::Pipeline& bind(IBindable<Bindable>& b)
        {
            b.bind(pipeline->get_binder());
            return *this;
        }

        /// Draw a single object with the current pipeline
        template <typename Drawable>
        RenderPass::Pipeline& draw(IDrawable<Drawable>& d)
        {
            d.draw();
            return *this;
        }

        /// Function to call raw render functions (OpenGL for example)
        template <typename Function>
        RenderPass::Pipeline& raw_render(Function f)
        {
            f();
            return *this;
        }

        template <typename Function>
        RenderPass::Pipeline& render(Function f)
        {
            f(*this);
            return *this;
        }
        template <typename Iter, typename Function>
        RenderPass::Pipeline& for_each(
            const Iter& begin,
            const Iter& end,
            Function function);

    private:
        Pipeline(RenderPass& _pass, RenderPipeline& _pipeline)
            : pass(&_pass)
            , pipeline(&_pipeline)
        {
            pipeline->use();
        }

        RenderPass* pass;
        RenderPipeline* pipeline = nullptr;
    };

    /// Clears state of all binded things and sets the current pipeline
    [[nodiscard]] RenderPass::Pipeline set_pipeline(RenderPipeline& p)
    {
        return Pipeline(*this, p);
    }

    /// Sets the viewport of the draw surface
    [[nodiscard]] RenderPass& viewport(const glm::ivec2& origin, const glm::ivec2& size);
    /// Sets the viewport to the whole surface
    [[nodiscard]] RenderPass& viewport_full();

    template <typename Iter, typename Function>
    RenderPass& for_each(
        const Iter& begin,
        const Iter& end,
        Function function);
};

// Template Implementations

template <typename Iter, typename Function>
inline RenderPass& RenderPass::for_each(
    const Iter& begin,
    const Iter& end,
    Function function)
{
    for (Iter it = begin; it != end; ++it) {
        function(*this, *it);
    }

    return *this;
}

template <typename Iter, typename Function>
inline RenderPass::Pipeline& RenderPass::Pipeline::for_each(
    const Iter& begin,
    const Iter& end,
    Function function)
{
    for (Iter it = begin; it != end; ++it) {
        function(*this, *it);
    }

    return *this;
}


}