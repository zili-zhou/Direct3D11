#include"Cube.hlsli"

//������ɫ��
float4 PS(VertexOut verticesInput) : SV_Target //˵���������ɫֵ����ֱ�ӱ��浽��ȾĿ����ͼ�ĺ󱸻�������Ӧλ��
{
    return verticesInput.color;
}