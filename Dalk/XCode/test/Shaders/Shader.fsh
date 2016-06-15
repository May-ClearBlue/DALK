//
//  Shader.fsh
//  test
//
//  Created by 中野 利哉 on 2012/08/31.
//  Copyright (c) 2012年 中野 利哉. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
