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

struct ShaderProgram::Impl final : public ShaderBinder {
    GLuint handle;
    std::vector<GLint> uniform_locations;

    unsigned int uniform_location(UniformID id) override
    {
        GFX_ASSERT(0 <= id && id < uniform_locations.size() && uniform_locations[id] >= 0,
            "UniformID must be in the requested shader");

        return uniform_locations[id];
    }

    ~Impl();
};

struct ShaderProgram::Builder::Impl {
    GLuint handle;
};

ShaderProgram::Builder::Builder(const std::string& _name)
    : glsl_version("#version 450\n\n")
    , name(_name)
    , constants()
    , uniform_ids()
    , max_uniform_id(0)
    , impl(new Impl)
{
    this->impl->handle = glCreateProgram();
}

ShaderProgram::Builder::~Builder()
{
}

ShaderProgram::Builder& ShaderProgram::Builder::with_version(unsigned int version)
{
    std::stringstream ss;
    ss << "#version " << version << "\n\n";
    this->glsl_version = ss.str();
    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_vertex_shader(const char* source)
{
    std::string tmp = constants.str() + "\n#line 0\n";
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
    std::string tmp = constants.str() + "\n#line 0\n";
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
    std::string tmp = constants.str() + "\n#line 0\n";
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
    this->constants << "#define " << name << " " << value << "\n";
    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_constant(const char* name, unsigned int value)
{
    this->constants << "#define " << name << " " << value << "\n";
    return *this;
}

ShaderProgram::Builder& ShaderProgram::Builder::with_constant(const char* name, float value)
{
    this->constants << "#define " << name << " " << value << "\n";
    return *this;
}

ShaderProgram ShaderProgram::Builder::build()
{
    glLinkProgram(this->impl->handle);

#ifdef GFX_VALIDATION
    _validate_program(this->impl->handle, this->name.data());
#endif

    std::vector<int> uniform_locations(max_uniform_id + 1, -1);
    for (auto& it : uniform_ids) {
        GLint location = glGetUniformLocation(this->impl->handle, it.first.c_str());
        GFX_ASSERT(location >= 0, "Requested uniform \"%s\" does not exist in shader \"%s\"", it.first.c_str(), this->name.c_str());

        uniform_locations[it.second] = location;
    }

    ShaderProgram program;
    program.impl->handle = this->impl->handle;
    program.impl->uniform_locations = std::move(uniform_locations);

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
    GFX_CHECK(this->impl->handle != 0, "Using an invalid ShaderProgram");
    glUseProgram(this->impl->handle);
}

// Do not confuse with bender ·-(8U|
ShaderBinder& ShaderProgram::get_binder()
{
    return *impl;
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