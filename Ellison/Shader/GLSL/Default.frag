static const char* DefaultFragmentShader = _STRINGIFY(

precision mediump float;

varying lowp vec4 Varying_Color;
varying mediump vec2 Varying_UV;

uniform sampler2D texture0;
 
void main()
{
    //gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	gl_FragColor = texture2D(texture0, Varying_UV) * Varying_Color;
}
);
