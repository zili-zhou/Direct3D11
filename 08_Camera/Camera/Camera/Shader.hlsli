#include"LightType.hlsli"

Texture2D g_Tex : register(t0);
SamplerState g_SamplerState: register(s0);

cbuffer CBChangesEveryDrawing : register(b0)
{
    matrix g_World;
    matrix g_WorldInvTranspose;//���ڷ��������ı任
}

cbuffer CBChangesEveryFrame : register(b1)
{
    matrix g_View;
    float3 EyePosW; //world����ϵ���۾�λ��
    
}
    
cbuffer CBChangesOnResize : register(b2)
{
    matrix g_Projection;
}

    
cbuffer CBChangesRarely : register(b3)
{
    DirectionalLight g_DirectionalLight[10];
    PointLight g_PointLight[10];
    SpotLight g_SpotLight[10];
     Material g_Matrial;
    int NumDirectionalLight;
    int NumPointLight;
    int NumSpotLight;
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