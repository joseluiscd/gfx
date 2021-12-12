#include <gfx/glad.h>
#include <gfx/log.hpp>
#include <gfx/texture_binding.hpp>

namespace gfx {

namespace texture::detail {
    void upload_texture(const TextureHandle& uniform, TextureUnit unit)
    {
#ifdef GFX_VALIDATION
        GLint max_texture_units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
        GFX_ASSERT(unit < max_texture_units, "Specified texture unit is not supported by OpenGL.");
#endif
        glBindTextureUnit(unit, uniform.get_id());
    }

}

}