#include"Shader.hlsli"

//像素着色器(2D)
//Texture2D类型拥有Sample方法，需要提供采样器状态和2D纹理坐标方可使用，然后返回一个包含RGBA信息的float4向量。
float4 PS_2D(VertexPosHTex vertexIn):SV_Target
{
    return g_Tex.Sample(g_SamplerState, vertexIn.TEXTURE);
}