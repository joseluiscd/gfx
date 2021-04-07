layout (location = kPosition) in vec2 v_position;
layout (location = kColor) in vec3 v_color;

out vec3 i_color;

void main()
{
    gl_Position = vec4(v_position, 0.0, 1.0);
    i_color = v_color;
}