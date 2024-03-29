#include <gfx/uniform.hpp>
#include <gfx/glad.h>

namespace gfx {

namespace uniform {
    // Not thread-safe
    UniformID generate_id(unsigned int size)
    {
        static UniformID next_location = 0;

        UniformID ret = next_location;
        next_location += size;
        return ret;
    }
}

namespace uniform::detail {
    void upload_uint(unsigned int uniform, unsigned int location)
    {
        glUniform1ui(location, uniform);
    }

    void upload_int(int uniform, unsigned int location)
    {
        glUniform1i(location, uniform);
    }

    void upload_float(float uniform, unsigned int location)
    {
        glUniform1f(location, uniform);
    }

    void upload_vec2(const glm::vec2& uniform, unsigned int location)
    {
        glUniform2fv(location, 1, &uniform[0]);
    }

    void upload_vec3(const glm::vec3& uniform, unsigned int location)
    {
        glUniform3fv(location, 1, &uniform[0]);
    }

    void upload_vec4(const glm::vec4& uniform, unsigned int location)
    {
        glUniform4fv(location, 1, &uniform[0]);
    }

    void upload_mat4(const glm::mat4& uniform, unsigned int location)
    {
        glUniformMatrix4fv(location, 1, false, &uniform[0][0]);
    }
};

}