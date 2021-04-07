layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_color;


layout (location = kProjectionMatrix) uniform mat4 mProj;
layout (location = kViewMatrix) uniform mat4 mView;

out vec4 i_color;
out vec4 i_normal;
out vec4 i_position;

void main()
{
    i_normal = mView * vec4(v_normal, 0.0);
    i_color = vec4(v_color, 1.0);
    i_position = mView * vec4(v_position, 1.0);
    gl_Position = mProj * mView * vec4(v_position, 1.0);
}