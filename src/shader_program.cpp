#include <fmt/format.h>
#include <fstream>
#include <gfx/glad.h>
#include <gfx/log.hpp>
#include <gfx/shader_program.hpp>

namespace gfx {

#ifdef GFX_VALIDATION
inline void _validate_shader(GLuint handle, const char* shader_type, const char* name);
inline void _validate_program(GLuint handle, const char* name);
#endif

std::string read_shader_file(const char* path);

struct ShaderProgram::Impl {
    GLuint handle;
    ~Impl();
};

struct ShaderProgram::Builder::Impl {
    GLuint handle;
};

ShaderProgram::Builder::Builder(const std::string& _name)
    : glsl_version("#version 450\n\n")
    , name(_name)
    , constants()
    , impl(new Impl)
{
    this->impl->handle = glCreateProgram();
}

ShaderProgram::Builder::~Builder()
{
}

ShaderProgram::Builder& ShaderProgram::Builder::with_version(unsigned int version)
{
    this->glsl_version = fmt::format("#version {}\n\n", version);
    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_vertex_shader(const char* source)
{
    std::string tmp = constants.str();
    const char* sources[] = { glsl_version.data(), tmp.data(), source };

    GLuint shader_handle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_handle, 3, sources, nullptr);
    glCompileShader(shader_handle);

#ifdef GFX_VALIDATION
    _validate_shader(shader_handle, "vertex", this->name.data());
#endif

    glAttachShader(this->impl->handle, shader_handle);

    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_fragment_shader(const char* source)
{
    std::string tmp = constants.str();
    const char* sources[] = { glsl_version.data(), tmp.data(), source };

    GLuint shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_handle, 3, sources, nullptr);
    glCompileShader(shader_handle);

#ifdef GFX_VALIDATION
    _validate_shader(shader_handle, "fragment", this->name.data());
#endif

    glAttachShader(this->impl->handle, shader_handle);

    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_geometry_shader(const char* source)
{
    std::string tmp = constants.str();
    const char* sources[] = { glsl_version.data(), tmp.data(), source };

    GLuint shader_handle = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(shader_handle, 3, sources, nullptr);
    glCompileShader(shader_handle);

#ifdef GFX_VALIDATION
    _validate_shader(shader_handle, "geometry", this->name.data());
#endif

    glAttachShader(this->impl->handle, shader_handle);

    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_vertex_shader_file(const char* path)
{
    return this->with_vertex_shader(read_shader_file(path).c_str());
}

ShaderProgram::Builder& ShaderProgram::Builder::with_fragment_shader_file(const char* path)
{
    return this->with_fragment_shader(read_shader_file(path).c_str());
}

ShaderProgram::Builder& ShaderProgram::Builder::with_geometry_shader_file(const char* path)
{
    return this->with_geometry_shader(read_shader_file(path).c_str());
}

ShaderProgram::Builder& ShaderProgram::Builder::with_constant(const char* name, int value)
{
    this->constants << fmt::format("#define {} {}\n", name, value);

    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_constant(const char* name, unsigned int value)
{
    this->constants << fmt::format("#define {} {}\n", name, value);

    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_constant(const char* name, float value)
{
    this->constants << fmt::format("#define {} {}\n", name, value);

    return *this;
}

ShaderProgram ShaderProgram::Builder::build()
{

    glLinkProgram(this->impl->handle);

#ifdef GFX_VALIDATION
    _validate_program(this->impl->handle, this->name.data());
#endif

    ShaderProgram program;
    program.impl->handle = this->impl->handle;

    return program;
}

ShaderProgram::Impl::~Impl()
{
    glDeleteProgram(this->handle);
}

ShaderProgram::ShaderProgram()
    : impl(new ShaderProgram::Impl)
{
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
    : impl(std::move(other.impl))
{
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::use()
{
    GFX_ASSERT(this->impl->handle != 0, "Using an invalid ShaderProgram");
    glUseProgram(this->impl->handle);
}

// File read
std::string read_shader_file(const char* path)
{
    std::ifstream source_file;
    source_file.open(path);
    if (!source_file) {
        GFX_ERROR("Cannot open shader source file: \"%s\"", path);
        return "";
    }

    std::stringstream read_string;
    read_string << source_file.rdbuf();

    return read_string.str();
}

// Validation functions

#ifdef GFX_VALIDATION
inline void _validate_shader(GLuint handle, const char* shader_type, const char* name)
{
    if (handle == 0) {
        GFX_ERROR("Error, invalid shader");
    }

    GLint compileResult;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compileResult);
    if (compileResult == GL_FALSE) {
        GLint logLen = 0;

        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char* log_string = new char[logLen];
            GLint written = 0;
            glGetShaderInfoLog(handle, logLen, &written, log_string);

            GFX_ERROR("Error compiling %s shader \"%s\":%s{}", shader_type, name, log_string);

            delete[] log_string;

        } else {
            GFX_ERROR("Could not compile %s shader \"%s\"", shader_type, name);
        }
    }
}

inline void _validate_program(GLuint handle, const char* name)
{
    GLint linkSuccess = 0;
    glGetProgramiv(handle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLint logLen = 0;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char* log_string = new char[logLen];
            GLint written = 0;
            glGetProgramInfoLog(handle, logLen, &written, log_string);
            GFX_ERROR("Error linking shader program \"%s\":\n%s", name, log_string);
            delete[] log_string;
        } else {
            GFX_ERROR("Could not link shader program \"%s\"", name);
        }
    }
}
#endif
}