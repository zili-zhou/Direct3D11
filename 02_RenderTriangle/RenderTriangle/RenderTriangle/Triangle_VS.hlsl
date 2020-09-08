#include"Triangle.hlsli"

Vertex_Out VS(Vertex_In vertexIn)
{
	Vertex_Out vertexOut;
	vertexOut.posH = float4(vertexIn.pos, 1.0f);
	vertexOut.color = vertexIn.color;
	return vertexOut;
}
