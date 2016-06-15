//
//  Shader.vsh
//  test
//
//  Created by 中野 利哉 on 2012/08/31.
//  Copyright (c) 2012年 中野 利哉. All rights reserved.
//

attribute vec4 position;
attribute vec4 color;
attribute vec2 texCoord0;

varying vec4 v_color;
varying vec2 v_texCoord0;

void main()
{
    v_color = color;
	v_texCoord0 = texCoord0;
    gl_Position = position;
}
