#pragma once

#include <variant>
#include <string>

namespace gfx {

enum class BindingType: bool {
    Static = false,
    Dynamic = true
};


class BindGroup {
public:
    BindGroup();
};

class UniformBlockLayout {
    struct Entry {
        std::string binding;
    };
};

}