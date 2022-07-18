#pragma once

#include <memory>
#include <vector>

namespace gfx {

/**
* @brief GPU buffer handle. Does not own its data.
* @note For an owning version, use `gfx::Buffer`.
*/
class BufferHandle {
public:
    BufferHandle();
    BufferHandle(const BufferHandle& other) = default;

    /// Build the buffer with a pointer and size in bytes
    BufferHandle(const void* buffer, size_t size_bytes, bool dynamic = false)
        : BufferHandle()
    {
        this->update_raw_buffer_bytes(buffer, size_bytes, dynamic);
    }

    virtual ~BufferHandle();

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

/**
* @brief Buffer that owns its data and handles GPU interoperability.
* @note For a not-owning version of a GPU buffer, use `gfx::BufferHandle`.
*
* To obtain a RawBuffer handle, use the method `gfx::Buffer::get_handle()`.
* @warni
*/
template <typename T>
class Buffer {
public:
    friend class Editor;

    /**
    * @brief Object that allows modifications on a gfx::Buffer and
    * uploads modified data to the GPU on destruction.
    */
    class Editor {
    public:
        friend class Buffer;

        /// Mutable access to buffer elements.
        T& operator[](size_t i) {
            return buffer->data[i];
        }

        /// Uploads modified data to the GPU.
        ~Editor()
        {
            buffer->update_buffer(dynamic);
            this->buffer = nullptr;
        }

        /// Get a mutable reference to the underlying vector for edit.
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

    BufferHandle& get_handle() { return raw_buffer; }
    const BufferHandle& get_handle() const { return raw_buffer; }

    const T& operator[](size_t i) const
    {
        return data[i];
    }

    size_t size() const
    {
        return data.size();
    }

    /**
    * @brief Get a handle to an Edit object to edit the contents of the buffer.
    * @return Object to modify the buffer and upload modified data to the GPU on destruction.
    */
    Editor edit(bool dynamic = true) {
        Editor ed;
        ed.buffer = this;
        ed.dynamic = dynamic;
        return ed;
    }

private:
    std::vector<T> data;
    BufferHandle raw_buffer;

    void update_buffer(bool dynamic = false);
};

template <typename T>
void BufferHandle::update_raw_buffer(T* buffer, size_t count, bool dynamic)
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
Buffer<T>::Buffer(const std::vector<T>& _data, bool dynamic)
    : data(_data)
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