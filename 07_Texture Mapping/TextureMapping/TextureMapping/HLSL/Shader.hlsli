#include"LightType.hlsli"

Texture2D g_Tex : register(t0);
SamplerState g_SamplerState: register(s0);

cbuffer VSConstantBuffer : register(b0)
{
    matrix g_World;
    matrix g_View;
    matrix g_Projection;
    matrix g_WorldInvTranspose;//���ڷ��������ı任
}


cbuffer PSConstantBuffer : register(b1)
{
    Material g_Matrial;
    DirectionalLight g_DirectionalLight[10];
    PointLight g_PointLight[10];
    SpotLight g_SpotLight[10];
    
    int NumDirectionalLight;
    int NumPointLight;
    int NumSpotLight;
    float g_Pad1;//16�ֽڴ������
    
    float3 EyePosW;//world����ϵ���۾�λ��
    float g_Pad2;
}

struct VertexPosNormalTex
{//L:local space
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TEXTURE : TEXCOORD;
};

struct VertexPosTex
{ //L:local space
    float3 PosL : POSITION;
    float2 TEXTURE : TEXCOORD;
};

struct VertexPosHWNormalWTex
{ //H:Homogenous space, W: World space
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;// �������е�λ��
    float3 NormalW : NORMAL;//���������������е�λ��
    float2 TEXTURE : TEXCOORD;
};

struct VertexPosHTex
{
    float4 PosH : SV_Position;//H:homogenous space
    float2 TEXTURE : TEXCOORD;
};