#include"Shader.hlsli"

//������ɫ��(2D)
//Texture2D����ӵ��Sample��������Ҫ�ṩ������״̬��2D�������귽��ʹ�ã�Ȼ�󷵻�һ������RGBA��Ϣ��float4������
float4 PS_2D(VertexPosHTex vertexIn):SV_Target
{
    return g_Tex.Sample(g_SamplerState, vertexIn.TEXTURE);
}