#include"LightType.hlsli"

cbuffer VSConstantBuffer : register(b0)
{
    matrix g_World;
    matrix g_View;
    matrix g_Projection;
    matrix g_WorldInvTranspose;//用于法线向量的变换
}


cbuffer PSConstantBuffer : register(b1)
{
    Material g_Matrial;
    DirectionalLight g_DirectionalLight;
    PointLight g_PointLight;
    SpotLight g_SpotLight;
    float3 EyePosW;//world坐标系中眼睛位置
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
    float3 PosW : POSITION; // 在世界中的位置
    float3 NormalW : NORMAL;  //法线向量在世界中的位置
    float4 Color : COLOR;
};