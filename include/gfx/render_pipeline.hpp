#pragma once

#include <gfx/render_surface.hpp>
#include <gfx/shader_program.hpp>
#include <gfx/bindable.hpp>
#include <glm/glm.hpp>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <functional>

namespace gfx {

template <typename CameraImpl>
class ICamera : public IBindable<CameraImpl>{
public:
    void bind(ShaderBinder& binder) {
        static_cast<CameraImpl*>(this)->bind(binder);
    };

    glm::mat4 get_matrix() {
        return static_cast<CameraImpl*>(this)->get_matrix();
    }
};

class AbstractCamera : public ICamera<AbstractCamera> {
public:
    virtual void bind_camera() = 0;
    virtual glm::mat4 get_matrix() = 0;
};

/// For using in an attribute, use AbstractCamera instead (has no template argument).
template <typename CameraImpl>
class DynamicCamera : public AbstractCamera, public ICamera<CameraImpl> {
public:
    DynamicCamera(CameraImpl& _impl) : impl(&_impl) {}

    virtual void bind_camera() override {
        impl->bind_camera();
    }

    virtual glm::mat4 get_matrix() override {
        return impl->get_matrix();
    } 
private:
    CameraImpl* impl;
};

template <typename Drawable>
class IDrawable {
public:
    void draw() {
        static_cast<Drawable*>(this)->draw();
    }
};

class AbstractDrawable {
public:
    virtual void draw() = 0;
};

/// For using in an attribute, use AbstractDrawable instead (has no template argument).
template <typename Drawable>
class DynamicDrawable : public AbstractDrawable, public IDrawable<Drawable> {
public:
    DynamicDrawable(Drawable& _impl) : impl(&_impl) {}

    virtual void draw() override
    {
        impl->draw();
    }
private:
    Drawable* impl;
};


class CullingStrategy {
    virtual void apply_culling() = 0;
};

class BlendingStrategy {
    virtual void apply_blending() = 0;
};


class RenderPipeline {
public:
    class Builder;
    friend class RenderHelper;

    RenderPipeline(const std::string& _label, std::shared_ptr<ShaderProgram> _shader,
        CullingStrategy* _culling,
        BlendingStrategy* _blending)
        : label(_label)
        , shader(_shader)
        , culling(_culling)
        , blending(_blending)
    {
    }

    ShaderBinder& get_binder()
    {
        return shader->get_binder();
    }

    void use();

private:
    std::string label;
    std::shared_ptr<ShaderProgram> shader;
    CullingStrategy* culling;
    BlendingStrategy* blending;
};

class RenderPipeline::Builder {
public:
    Builder(const std::string& _label = "")
        : label(_label)
        , shader(nullptr)
        , culling(nullptr)
        , blending(nullptr)
    {
    }

    Builder& with_shader(std::shared_ptr<ShaderProgram> _shader)
    {
        shader = _shader;
        return *this;
    }

    Builder& with_shader(ShaderProgram&& _shader)
    {
        this->shader = std::shared_ptr<ShaderProgram>(new ShaderProgram(std::move(_shader)));
        return *this;
    }

    /// TODO: Check if it is valid
    RenderPipeline build();

    std::unique_ptr<RenderPipeline> build_unique()
    {
        return std::unique_ptr<RenderPipeline>(new RenderPipeline(this->build()));
    }

    std::shared_ptr<RenderPipeline> build_shared()
    {
        return std::shared_ptr<RenderPipeline>(new RenderPipeline(this->build()));
    }

private:
    std::string label;
    std::shared_ptr<ShaderProgram> shader;
    CullingStrategy* culling;
    BlendingStrategy* blending;
};



} // namespace gfx