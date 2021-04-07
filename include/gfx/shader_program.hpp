#pragma once

#include <gfx/uniform.hpp>
#include <memory>
#include <sstream>
#include <string>

namespace gfx {

/// Handle to a Shader program.
/// It is NOT copyable.
class ShaderProgram {
public:
    class Builder;
    friend class Builder;

    ShaderProgram(ShaderProgram&& other);
    ShaderProgram(const ShaderProgram& other) = delete;

    ~ShaderProgram();

    void use();

private:
    ShaderProgram();

    class Impl;
    std::unique_ptr<Impl> impl;
};

class ShaderProgram::Builder {
public:
    Builder(const std::string& name = "");
    ~Builder();

    Builder& with_version(unsigned int version);

    Builder& with_vertex_shader(const char* source);
    Builder& with_fragment_shader(const char* source);
    Builder& with_geometry_shader(const char* source);

    Builder& with_vertex_shader_file(const char* path);
    Builder& with_fragment_shader_file(const char* path);
    Builder& with_geometry_shader_file(const char* path);

    Builder& with_constant(const char* name, int value);
    Builder& with_constant(const char* name, unsigned int value);
    Builder& with_constant(const char* name, float value);

    template <typename Semantics>
    Builder& register_uniform()
    {
        return this->with_constant(Semantics::location, Uniform<Semantics>::location);
    }

    template <typename Registrable>
    Builder& register_class()
    {
        Registrable::register_shader(*this);
        return *this;
    }

    ShaderProgram build();

    std::unique_ptr<ShaderProgram> build_unique()
    {
        return std::unique_ptr<ShaderProgram>(new ShaderProgram(this->build()));
    }

    std::shared_ptr<ShaderProgram> build_shared()
    {
        return std::shared_ptr<ShaderProgram>(new ShaderProgram(this->build()));
    }

private:
    std::string glsl_version;
    std::string name;
    std::stringstream constants;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
