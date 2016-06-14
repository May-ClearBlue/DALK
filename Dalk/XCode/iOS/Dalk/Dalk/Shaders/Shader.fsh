//
//  Shader.fsh
//  Dalk
//
//  Created by まひる。 on 2016/06/08.
//  Copyright © 2016年 May.Watasumi. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
