typedef float4x4 mat4;

// -------------------------------------------------------------
// �O���[�o���ϐ�
// -------------------------------------------------------------
uniform int diffY : register(i0);
uniform int offsetX : register(i1);
uniform int offsetY : register(i2);
uniform mat4 world_matrix : register(c0);
uniform mat4 view_matrix :  register(c4);
uniform mat4 projection_matrix : register(c8);

// -------------------------------------------------------------
// ���_�V�F�[�_����s�N�Z���V�F�[�_�ɓn���f�[�^
// -------------------------------------------------------------

// ���_�V�F�[�_�[�̏o�̓p�����[�^
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
// ���_�V�F�[�_�v���O�����i�e�N�X�`���L��j
// -------------------------------------------------------------
VS_OUTPUT main (
    float3 pos    : POSITION,		// ���_���W
    float4 color   : COLOR,			// ���_�J���[
    float2 tex    : TEXCOORD0		// �e�N�X�`�����W
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;		// �o�̓f�[�^
    mat4 matWVP = world_matrix * view_matrix * projection_matrix;
    Out.Pos = mul( float4(pos.xyz,1.0f), matWVP );	// �ʒu���W
    Out.Col = color;					// ���_�J���[
    Out.Tex = tex;						// �e�N�X�`�����W

    return Out;
}



//�s�N�Z���V�F�[�_�i�t���O�����g�V�F�[�_�j

// -------------------------------------------------------------
// �e�N�X�`��
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
// �s�N�Z���V�F�[�_�v���O�����i�e�N�X�`���L��j
// -------------------------------------------------------------
float4 main ( VS_OUTPUT In ) : COLOR
{	
    float4 Out;
	
    Out = tex2D( Sampler, In.Tex ) * In.Col;
	
    return Out;
}
