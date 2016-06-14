static const char* NoTextureVertexShader = _STRINGIFY(

uniform int diffY;
uniform int offsetX;
uniform int offsetY;
uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

attribute vec3 position;
attribute vec4 diffuse;

varying lowp	vec4 Varying_Color;

void main(void)
{
    Varying_Color = diffuse;
    gl_Position = projection_matrix * view_matrix * world_matrix * vec4(position,1.0);
}
);
