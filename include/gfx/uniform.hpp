#pragma once

#include <gfx/bindable.hpp>
#include <glm/glm.hpp>

namespace gfx {

#define DEF_UNIFORM_SEMANTICS(CLS, TYPE, LOC)           \
    struct CLS : public gfx::UniformSemantics {         \
        using type = TYPE;                              \
        static constexpr const char* location = (LOC); \
    };

namespace uniform {
    /// This is not thread-safe
    UniformID generate_location(unsigned int size = 1);
}

/// A type that can be uploaded to the GPU as a uniform (glm::vec3)
template <typename T>
struct UniformType {
    const static unsigned int n_locations = 1;
    static void upload(const T& uniform, unsigned int location);
};

/// Semantics of the uniform (e.g. CameraProjectionMatrix, TextureSamplerColor0)
struct UniformSemantics {
    using type = void;
};

/// A uniform value inside a class
template <typename Semantics>
class Uniform : public IBindable<Uniform<Semantics>> {
    using Type = typename Semantics::type;

    static_assert(
        std::is_convertible<
            Semantics,
            UniformSemantics>::value,
        "Type does not inherit UniformSemantics");
    static_assert(
        std::is_same_v<decltype(UniformType<Type>::upload), void(const Type&, unsigned int)>,
        "Type is not a valid uniform type.\n"
        "You must define `static void gfx::UniformType<Type>::upload(const Type& value, unsigned int location)` for this type.");

public:
    static const UniformID uniform_id;

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

    void bind(ShaderBinder& binder)
    {
        UniformType<Type>::upload(value, binder.uniform_location(uniform_id));
    }

private:
    Type value;
};

template <typename Semantics>
const UniformID Uniform<Semantics>::uniform_id = []() {
    return gfx::uniform::generate_location(UniformType<typename Semantics::type>::n_locations);
}();

namespace uniform::detail {
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
    const static unsigned int n_locations = 1;
    static void upload(const unsigned int& uniform, unsigned int location)
    {
        uniform::detail::upload_uint(uniform, location);
    }
};

template <>
struct UniformType<int> {
    const static unsigned int n_locations = 1;
    static void upload(const int& uniform, unsigned int location)
    {
        uniform::detail::upload_int(uniform, location);
    }
};

template <>
struct UniformType<float> {
    const static unsigned int n_locations = 1;
    static void upload(const float& uniform, unsigned int location)
    {
        uniform::detail::upload_float(uniform, location);
    }
};

template <>
struct UniformType<glm::vec2> {
    const static unsigned int n_locations = 1;
    static void upload(const glm::vec2& uniform, unsigned int location)
    {
        uniform::detail::upload_vec2(uniform, location);
    }
};

template <>
struct UniformType<glm::vec3> {
    const static unsigned int n_locations = 1;
    static void upload(const glm::vec3& uniform, unsigned int location)
    {
        uniform::detail::upload_vec3(uniform, location);
    }
};

template <>
struct UniformType<glm::vec4> {
    const static unsigned int n_locations = 1;
    static void upload(const glm::vec4& uniform, unsigned int location)
    {
        uniform::detail::upload_vec4(uniform, location);
    }
};

template <>
struct UniformType<glm::mat4> {
    const static unsigned int n_locations = 1;
    static void upload(const glm::mat4& uniform, unsigned int location)
    {
        uniform::detail::upload_mat4(uniform, location);
    }
};

}