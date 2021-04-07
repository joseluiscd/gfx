#include <gfx/glad.h>
#include <gfx/texture.hpp>

namespace gfx {

GLenum TextureType_get_GL_internal(TextureType t);
GLenum TextureType_get_GL(TextureType t);
GLenum TextureType_get_GL_type(TextureType t);

}