#include"Triangle.hlsli"

float4 PS(Vertex_Out pixelIn) : SV_Target //˵���������ɫֵ����ֱ�ӱ��浽��ȾĿ����ͼ�ĺ󱸻�������Ӧλ��
{
	return pixelIn.color;
}