#include"Triangle.hlsli"

float4 PS(Vertex_Out pixelIn) : SV_Target //说明输出的颜色值将会直接保存到渲染目标视图的后备缓冲区对应位置
{
	return pixelIn.color;
}