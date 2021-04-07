#pragma once

namespace gfx {

template <typename Bindable>
class IBindable {
public:
    void bind() {
        static_cast<Bindable*>(this)->bind();
    }
};

class AbstractBindable {
public:
    virtual void bind() = 0;
};

template <typename Bindable>
class DynamicBindable : public AbstractBindable, public IBindable<Bindable>{
public:
    DynamicBindable(Bindable& _impl) : impl(&_impl) {}
    void bind() override
    {
        impl->bind();
    }
private:
    Bindable* impl;
};


}