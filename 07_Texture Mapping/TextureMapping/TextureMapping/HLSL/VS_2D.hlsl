#include"Shader.hlsli"

//2D������ɫ��
VertexPosHTex VS_2D(VertexPosTex vertexIn)
{
    VertexPosHTex vertexOut;
    vertexOut.PosH = float4(vertexIn.PosL, 1.0f);
    vertexOut.TEXTURE = vertexIn.TEXTURE;
    return vertexOut;
}