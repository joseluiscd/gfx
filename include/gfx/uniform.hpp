#pragma once

#include <array>
#include <glm/glm.hpp>
#include <gfx/bindable.hpp>

namespace gfx {

#define DEF_UNIFORM_SEMANTICS(CLS, TYPE, LOC)           \
    struct CLS : public gfx::UniformSemantics {         \
        using type = TYPE;                              \
        static constexpr const char* location = (LOC); \
    };

namespace uniform {
    /// This is not thread-safe
    unsigned int generate_location(unsigned int size = 1);
}

/// A type that can be uploaded to the GPU as a uniform (glm::vec3)
template <typename T>
struct UniformType {
    static const bool defined = false;
    const static unsigned int n_locations = 1;
    static void upload(const T& uniform, unsigned int location);
};

/// Semantics of the uniform (e.g. CameraProjectionMatrix, TextureSamplerColor0)
struct UniformSemantics {
    using type = void;
    static constexpr const char* location = "#error asdf";
    const static unsigned int n_locations = 1;
};

/// A uniform value inside a class
template <typename Semantics>
class Uniform : public IBindable<Uniform<Semantics>> {
    static_assert(
        std::is_convertible<
            Semantics,
            UniformSemantics>::value,
        "Type does not inherit UniformSemantics");
    static_assert(
        UniformType<typename Semantics::type>::defined,
        "UniformSemantics::type is not a valid uniform type");

    using Type = typename Semantics::type;

public:
    static const unsigned int location;

    Uniform()
        : value()
    {
    }

    Uniform(const Type& _value)
        : value(_value)
    {
    }

    Type& operator*()
    {
        return this->value;
    }

    const Type& operator*() const
    {
        return this->value;
    }

    void bind()
    {
        UniformType<Type>::upload(value, location);
    }

private:
    Type value;
};

template <typename Semantics>
const unsigned int Uniform<Semantics>::location = []() {
    return gfx::uniform::generate_location(Semantics::n_locations);
}();

namespace _impl_UniformType {
    void upload_uint(unsigned int uniform, unsigned int location);
    void upload_int(int uniform, unsigned int location);
    void upload_float(float uniform, unsigned int location);
    void upload_vec2(const glm::vec2& uniform, unsigned int location);
    void upload_vec3(const glm::vec3& uniform, unsigned int location);
    void upload_vec4(const glm::vec4& uniform, unsigned int location);
    void upload_mat4(const glm::mat4& uniform, unsigned int location);
};

template <>
struct UniformType<unsigned int> {
    static const bool defined = true;
    static void upload(const unsigned int& uniform, unsigned int location)
    {
        _impl_UniformType::upload_uint(uniform, location);
    }
};

template <>
struct UniformType<int> {
    static const bool defined = true;
    static void upload(const int& uniform, unsigned int location)
    {
        _impl_UniformType::upload_int(uniform, location);
    }
};

template <>
struct UniformType<float> {
    static const bool defined = true;
    static void upload(const float& uniform, unsigned int location)
    {
        _impl_UniformType::upload_float(uniform, location);
    }
};

template <>
struct UniformType<glm::vec2> {
    static const bool defined = true;
    static void upload(const glm::vec2& uniform, unsigned int location)
    {
        _impl_UniformType::upload_vec2(uniform, location);
    }
};

template <>
struct UniformType<glm::vec3> {
    static const bool defined = true;
    static void upload(const glm::vec3& uniform, unsigned int location)
    {
        _impl_UniformType::upload_vec3(uniform, location);
    }
};

template <>
struct UniformType<glm::vec4> {
    static const bool defined = true;
    static void upload(const glm::vec4& uniform, unsigned int location)
    {
        _impl_UniformType::upload_vec4(uniform, location);
    }
};

template <>
struct UniformType<glm::mat4> {
    static const bool defined = true;
    static void upload(const glm::mat4& uniform, unsigned int location)
    {
        _impl_UniformType::upload_mat4(uniform, location);
    }
};

}