in vec4 i_color;
in vec4 i_normal;
in vec4 i_position;

out vec4 f_color;

void main()
{
    f_color = max(dot(vec3(0.0, 0.0, 1.0), normalize(vec3(i_normal))), 0.0) * i_color;
}