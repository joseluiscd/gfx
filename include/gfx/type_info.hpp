#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <string>

namespace gfx {

enum PrimitiveType : size_t {
    Byte,
    UnsignedByte,
    Float,
    Double,
    Int16,
    Int32,
    Unsigned16,
    Unsigned32,
};

constexpr const size_t PrimitiveType_size[] = {
    sizeof(std::byte), // Byte
    sizeof(std::byte), // UnsignedByte
    sizeof(float), // Float
    sizeof(double), // Double
    sizeof(std::int16_t), // Int16
    sizeof(std::int32_t), // Int32
    sizeof(std::uint16_t), // Unsigned16
    sizeof(std::uint32_t), // Unsigned32
};

template <typename Element>
struct PrimitiveTypeInfo;

template <>
struct PrimitiveTypeInfo<int8_t> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Byte;
};

template <>
struct PrimitiveTypeInfo<uint8_t> {
    static constexpr const PrimitiveType type_id = PrimitiveType::UnsignedByte;
};

template <>
struct PrimitiveTypeInfo<float> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Float;
};

template <>
struct PrimitiveTypeInfo<double> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Double;
};

template <>
struct PrimitiveTypeInfo<int16_t> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Int16;
};

template <>
struct PrimitiveTypeInfo<uint16_t> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Unsigned16;
};

template <>
struct PrimitiveTypeInfo<int32_t> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Int32;
};

template <>
struct PrimitiveTypeInfo<uint32_t> {
    static constexpr const PrimitiveType type_id = PrimitiveType::Unsigned32;
};

/// Type info for vector types (glm::vec3).
/// Primitive types are considered as a vector of only one component.
template <typename Element, typename = void>
struct VectorTypeInfo;

template <typename Element>
struct VectorTypeInfo<Element, std::void_t<PrimitiveTypeInfo<Element>>> {
    using base_type = Element;
    static constexpr const size_t count = 1;
};

template <typename T, size_t L>
struct VectorTypeInfo<glm::vec<L, T>> {
    using base_type = T;
    static constexpr const size_t count = L;
};

}