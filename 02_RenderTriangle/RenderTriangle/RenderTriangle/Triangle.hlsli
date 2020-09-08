struct Vertex_In
{
	float3 pos:POSITION;
	float4 color : COLOR;
};

struct Vertex_Out
{
	float4 posH : SV_POSITION; //说明该顶点的位置在从顶点着色器输出后，后续的着色器都不能改变它的值，作为光栅化时最终确定的像素位置
	float4 color : COLOR;
};