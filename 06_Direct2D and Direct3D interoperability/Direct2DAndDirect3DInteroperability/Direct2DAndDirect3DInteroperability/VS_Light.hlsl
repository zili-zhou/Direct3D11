#include"Light.hlsli"

VertexOutput VS(VertexInput verticesIn)
{
    VertexOutput verticesOut;
    float4 posW = mul(float4(verticesIn.PosL, 1.0f), g_World);
    matrix ViewProj = mul(g_View, g_Projection);
    
    verticesOut.PosH = mul(posW, ViewProj);
    verticesOut.PosW = posW.xyz;
    verticesOut.NormalW = mul(verticesIn.NormalL, (float3x3) g_WorldInvTranspose); // 只取4x4矩阵的前3行前3列
    verticesOut.Color = verticesIn.Color;
    return verticesOut;
}