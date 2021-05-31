#pragma once

#include <memory>
#include <vector>

namespace gfx {

/// This raw buffer doesn't own the data
class RawBuffer {
public:
    RawBuffer();
    RawBuffer(const RawBuffer& other) = default;

    /// Build the buffer with a pointer and size in bytes
    RawBuffer(const void* buffer, size_t size_bytes, bool dynamic = false)
        : RawBuffer()
    {
        this->update_raw_buffer_bytes(buffer, size_bytes, dynamic);
    }

    virtual ~RawBuffer();

    void bind_vertex_array() const;
    void bind_element_array() const;

    unsigned int get_id() const;

    void update_raw_buffer(bool dynamic = false);
    
    template <typename T>
    void update_raw_buffer(T* buffer, size_t count, bool dynamic = false);

    void update_raw_buffer_bytes(const void* buffer, size_t size, bool dynamic = false);

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

/// This buffer owns the data
template <typename T>
class Buffer {
public:
    friend class Editor;
    class Editor {
    public:
        friend class Buffer;
        T& operator[](size_t i) {
            return buffer->data[i];
        }

        ~Editor()
        {
            buffer->update_buffer(dynamic);
            this->buffer = nullptr;
        }

        std::vector<T>& vector() {
            return buffer->data;
        }


    private:
        Buffer<T>* buffer;
        bool dynamic;
    };

    Buffer();
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) = default;
    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) = default;

    /// Copies data
    Buffer(T* buffer, size_t count, bool dynamic = false);
    /// Copies data
    Buffer(const std::vector<T>& data, bool dynamic = false);

    /// Moves data
    Buffer(std::vector<T>&& data, bool dynamic = false);

    ~Buffer();

    /// Copies data
    void update_buffer(T* buffer, size_t count, bool dynamic = false);
    /// Copies data
    void update_buffer(const std::vector<T>& data, bool dynamic = false);

    ///Moves data
    void update_buffer(std::vector<T>&& data, bool dynamic = false);

    RawBuffer& get_raw() { return raw_buffer; }
    const RawBuffer& get_raw() const { return raw_buffer; }

    const T& operator[](size_t i) const
    {
        return data[i];
    }

    size_t size() const
    {
        return data.size();
    }

    Editor edit(bool dynamic = true) {
        Editor ed;
        ed.buffer = this;
        ed.dynamic = dynamic;
        return ed;
    }

private:
    std::vector<T> data;
    RawBuffer raw_buffer;

    void update_buffer(bool dynamic = false);
};

template <typename T>
void RawBuffer::update_raw_buffer(T* buffer, size_t count, bool dynamic)
{
    update_raw_buffer_bytes((void*) buffer, count * sizeof(T), dynamic);
}

template <typename T>
Buffer<T>::Buffer()
    : data()
    , raw_buffer()
{
    this->update_buffer();
}

template <typename T>
Buffer<T>::Buffer(std::vector<T>&& _data, bool dynamic)
    : data(std::move(_data))
    , raw_buffer()
{
    this->update_buffer(dynamic);
}

template <typename T>
void Buffer<T>::update_buffer(T* buffer, size_t count, bool dynamic)
{
    this->data = std::move(std::vector<T>(buffer, buffer+count));
    this->update_buffer(dynamic);
}

template <typename T>
void Buffer<T>::update_buffer(const std::vector<T>& data, bool dynamic)
{
    this->data = data;
    this->update_buffer(dynamic);
}

template <typename T>
void Buffer<T>::update_buffer(std::vector<T>&& data, bool dynamic)
{
    this->data = std::move(data);
    this->update_buffer(dynamic);
}

template <typename T>
void Buffer<T>::update_buffer(bool dynamic)
{
    raw_buffer.update_raw_buffer(data.data(), data.size(), dynamic);
}

template <typename T>
Buffer<T>::~Buffer()
{
}

}