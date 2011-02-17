//
//  Shader.fsh
//  iPhoneTest
//
//  Created by work on 2/17/11.
//  Copyright __MyCompanyName__ 2011. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
