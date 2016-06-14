static const char* DefaultVertexShader = _STRINGIFY(

uniform int diffY;
uniform int offsetX;
uniform int offsetY;
uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

attribute vec3 position;
attribute vec4 diffuse;
attribute vec2 uv0;

varying lowp	vec4 Varying_Color;
varying mediump vec2 Varying_UV;

//gl_TexCoord[0]

void main(void)
{
 	Varying_Color = diffuse;
	Varying_UV = uv0;
 	gl_Position = projection_matrix * view_matrix * world_matrix * vec4(position,1.0);
}
);
