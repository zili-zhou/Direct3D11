#include"Shader.hlsli"

// ������ɫ��(3D)
VertexPosHWNormalWTex VS_3D(VertexPosNormalTex vertexIn)
{
    VertexPosHWNormalWTex vertexOut;
    float4 posW = mul(float4(vertexIn.PosL, 1.0f), g_World);
    matrix ViewProj = mul(g_View, g_Projection);
    
    vertexOut.PosH = mul(posW, ViewProj);
    vertexOut.PosW = posW.xyz;
    vertexOut.NormalW = mul(vertexIn.NormalL, (float3x3) g_WorldInvTranspose); // ֻȡ4x4�����ǰ3��ǰ3��
    vertexOut.TEXTURE = vertexIn.TEXTURE;
    return vertexOut;
}