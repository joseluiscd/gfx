
layout (points) in;
in vec3 i_color[];

layout (triangle_strip, max_vertices=3) out;
out vec3 v_color;

void main()
{
    vec4 position = gl_in[0].gl_Position;
    vec3 color = i_color[0];

    gl_Position = position + vec4(0.1, -0.1, 0.0, 0.0);
    v_color = color + vec3(0.5, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.1, 0.0, 0.0);
    v_color = color + vec3(0.0, 0.5, 0.0);
    EmitVertex();
    gl_Position = position + vec4(-0.1, -0.1, 0.0, 0.0);
    v_color = color + vec3(0.0, 0.0, 0.5);
    EmitVertex();

    EndPrimitive();

}