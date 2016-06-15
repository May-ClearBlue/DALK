typedef float4x4 mat4;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------
uniform int diffY : register(i0);
uniform int offsetX : register(i1);
uniform int offsetY : register(i2);
uniform mat4 world_matrix : register(c0);
uniform mat4 view_matrix :  register(c4);
uniform mat4 projection_matrix : register(c8);

// -------------------------------------------------------------
// 頂点シェーダからピクセルシェーダに渡すデータ
// -------------------------------------------------------------

// 頂点シェーダーの出力パラメータ
struct VS_INPUT {
	float3 pos   : POSITION;
	float4 color : COLOR;
	float2 uv    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Col			: COLOR;
    float2 Tex			: TEXCOORD0;
};

// -------------------------------------------------------------
// 頂点シェーダプログラム（テクスチャ有り）
// -------------------------------------------------------------
VS_OUTPUT main (
    float3 pos    : POSITION,		// 頂点座標
    float4 color   : COLOR,			// 頂点カラー
    float2 tex    : TEXCOORD0		// テクスチャ座標
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;		// 出力データ
    mat4 matWVP = world_matrix * view_matrix * projection_matrix;
    Out.Pos = mul( float4(pos.xyz,1.0f), matWVP );	// 位置座標
    Out.Col = color;					// 頂点カラー
    Out.Tex = tex;						// テクスチャ座標

    return Out;
}



//ピクセルシェーダ（フラグメントシェーダ）

// -------------------------------------------------------------
// テクスチャ
// -------------------------------------------------------------

//uniform sampler2D texture0;
//texture Tex : TEXTURE0;
texture Tex : register(t0);
sampler Sampler : register(s0) = sampler_state {
    Texture = <Tex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

// -------------------------------------------------------------
// ピクセルシェーダプログラム（テクスチャ有り）
// -------------------------------------------------------------
float4 main ( VS_OUTPUT In ) : COLOR
{	
    float4 Out;
	
    Out = tex2D( Sampler, In.Tex ) * In.Col;
	
    return Out;
}
