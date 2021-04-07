in vec3 i_color;

out vec4 f_color;

void main()
{
    f_color = vec4(i_color, 1.0);
}