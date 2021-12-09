#pragma once

#include <cstddef>

namespace gfx {

/// An uniform in our program. Values are usually an enum variant.
using UniformID = size_t;

/// Not an object that binds something to a shader, but the one who knows where to put it (locations).
class ShaderBinder {
public:
    virtual unsigned int uniform_location(UniformID id) = 0; 
};

/// Interface for objects that can be binded to a shader
template <typename Bindable>
class IBindable {
public:
    void bind(ShaderBinder& binder) {
        static_cast<Bindable*>(this)->bind(binder);
    }
};

class AbstractBindable {
public:
    virtual void bind(ShaderBinder& binder) = 0;
};

template <typename Bindable>
class DynamicBindable : public AbstractBindable, public IBindable<Bindable>{
public:
    DynamicBindable(Bindable& _impl) : impl(&_impl) {}
    void bind(ShaderBinder& binder) override
    {
        impl->bind(binder);
    }
private:
    Bindable* impl;
};


}