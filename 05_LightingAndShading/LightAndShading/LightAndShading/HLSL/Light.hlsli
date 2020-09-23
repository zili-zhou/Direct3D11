#include"LightType.hlsli"

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
    DirectionalLight g_DirectionalLight;
    PointLight g_PointLight;
    SpotLight g_SpotLight;
    float3 EyePosW;//world����ϵ���۾�λ��
    float g_Pad;
}

struct VertexInput
{//L:local space
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float4 Color : COLOR;
};

struct VertexOutput
{
    float4 PosH : SV_Position;//H:homogenous space
    float3 PosW : POSITION; // �������е�λ��
    float3 NormalW : NORMAL;  //���������������е�λ��
    float4 Color : COLOR;
};