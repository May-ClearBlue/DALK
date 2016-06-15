//
//  shader.h
//  test
//
//  Created by 中野 利哉 on 2012/08/31.
//  Copyright (c) 2012年 中野 利哉. All rights reserved.
//

BOOL SetupShader();
//void SetShader(UINT32 sno, GLKMatrix4 *mat);
void SetShader(UINT32 sno, float defy, float ofsx, float ofsy);
void SetTLVertex(float *pbuf);
void SetLVertex(float *pbuf);
void SetVertex(float *pbuf);

void DrawPrimitiveTRISTRIP(UINT32 st, UINT32 ct);
void DrawPrimitiveTRI(UINT32 st, UINT32 ct);

void SetTexture(UINT32 shno, UINT32 id, UINT32 texid);

