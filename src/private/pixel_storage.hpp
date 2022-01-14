#include <gfx/glad.h>
#include <gfx/texture.hpp>

namespace gfx {

GLenum TextureFormat_get_GL_internal(TextureFormat t);
GLenum TextureFormat_get_GL(TextureFormat t);
GLenum TextureFormat_get_GL_type(TextureFormat t);

}