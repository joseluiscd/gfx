#pragma once

#include <gfx/uniform.hpp>
#include <gfx/texture_binding.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

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
    ShaderBinder& get_binder();

private:
    ShaderProgram();

    struct Impl;
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

    // Register uniform id at the specified location
    Builder& register_uniform(const char* location, UniformID id)
    {
        uniform_ids.push_back(std::make_pair(location, id));
        if (id > max_uniform_id) {
            max_uniform_id = id;
        }

        return *this;
    }

    // Register uniform type at the specified location
    template <typename T>
    Builder& register_uniform(const char* location)
    {
        if constexpr (std::is_base_of_v<UniformSemantics, T>) {
            return register_uniform(location, Uniform<T>::uniform_id);
        } else {
            return register_uniform(location, T::uniform_id);
        }
    }

    template <TextureUnit UNIT>
    Builder& register_texture(const char* location)
    {
        return register_uniform<TextureBinding<UNIT>>(location);
    }

    /// Register class with the shader. The arguments `args` depend on the registered class `Registrable`
    template <typename Registrable, typename... Args>
    Builder& register_class(Args&&... args)
    {
        Registrable::register_shader(*this, std::forward<Args>(args)...);
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

    std::vector<std::pair<std::string, size_t>> uniform_ids;
    UniformID max_uniform_id;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
