#pragma once

#include <gfx/render_pipeline.hpp>
#include <gfx/uniform.hpp>
#include <glm/glm.hpp>

namespace gfx {

class CameraLens : public IResizeObserver, public ICamera<CameraLens> {
public:
    struct ProjectionMatrix : public UniformSemantics {
        using type = glm::mat4;
    };

    CameraLens()
        : proj_matrix()
    {
    }

    CameraLens(const glm::mat4& projection)
        : proj_matrix(projection)
    {
    }

    virtual void set_aspect(float _aspect) = 0;
    virtual void resize_event(const glm::ivec2& new_size) override
    {
        set_aspect(float(new_size.x) / float(new_size.y));
    }

    glm::mat4& projection_matrix() { return *proj_matrix; }
    const glm::mat4& projection_matrix() const { return *proj_matrix; }

    void bind(ShaderBinder& binder)
    {
        proj_matrix.bind(binder);
    }

    glm::mat4 get_matrix() //override
    {
        return *proj_matrix;
    }

    static void register_shader(ShaderProgram::Builder& builder, const char* location)
    {
        builder.register_uniform<ProjectionMatrix>(location);
    }

protected:
    Uniform<ProjectionMatrix> proj_matrix;
};

class PerspectiveCameraLens : public CameraLens {
public:
    PerspectiveCameraLens(float _fovy, float _aspect, float _znear, float _zfar)
        : fovy(_fovy)
        , aspect(_aspect)
        , znear(_znear)
        , zfar(_zfar)
        , CameraLens()
    {
        update_matrix();
    }

    void set_fovy(float _fovy)
    {
        fovy = _fovy;
        update_matrix();
    }
    void set_aspect(float _aspect)
    {
        aspect = _aspect;
        update_matrix();
    }
    void set_znear(float _znear)
    {
        znear = _znear;
        update_matrix();
    }
    void set_zfar(float _zfar)
    {
        zfar = _zfar;
        update_matrix();
    }

    void zoom(float zoom)
    {
        fovy = glm::clamp(zoom * fovy, 0.0f, 180.0f);
        update_matrix();
    }

private:
    void update_matrix();
    float fovy, aspect, znear, zfar;
};

class CameraRig : public ICamera<CameraRig> {
public:
    struct ViewMatrix : public UniformSemantics {
        using type = glm::mat4;
    };

    CameraRig(CameraLens& __lens)
        : _lens(&__lens)
        , v_position(0.0, 0.0, 1.0)
        , v_lookat(0.0, 0.0, 0.0)
        , v_up(0.0, 1.0, 0.0)
    {
    }

    CameraRig(const CameraRig& other) = default;
    CameraRig(CameraRig&& other) = default;

    CameraLens& lens() { return *_lens; }
    const CameraLens& lens() const { return *_lens; }

    CameraRig& look_at(const glm::vec3& v)
    {
        v_lookat = v;
        update_matrix();
        return *this;
    }
    CameraRig& with_position(const glm::vec3& v)
    {
        v_position = v;
        update_matrix();
        return *this;
    }

    CameraRig& with_up_vector(const glm::vec3& v)
    {
        v_up = v;
        update_matrix();
        return *this;
    }

    /// Binds this camera rig and the lens
    void bind(ShaderBinder& binder) //override
    {
        _lens->bind(binder);
        view_matrix.bind(binder);
    }

    glm::mat4 get_matrix() //override
    {
        return _lens->get_matrix() * (*view_matrix);
    }

    static void register_shader(ShaderProgram::Builder& builder, const char* location)
    {
        builder.register_uniform<ViewMatrix>(location);
    }

    //Movement
    void pan(float angle);
    void tilt(float angle);
    void dolly(float distance);
    void truck(float distance);
    void crane(float distance);
    void vertical(float distance);
    void advance(float distance);
    void orbit(float angle);
    void orbit_vertical(float angle);

private:
    void update_matrix();

    Uniform<ViewMatrix> view_matrix;

    CameraLens* _lens;

    glm::vec3 u, v, n;

    glm::vec3 v_position;
    glm::vec3 v_lookat;
    glm::vec3 v_up;
};

}