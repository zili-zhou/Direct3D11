struct Vertex_In
{
	float3 pos:POSITION;
	float4 color : COLOR;
};

struct Vertex_Out
{
	float4 posH : SV_POSITION; //˵���ö����λ���ڴӶ�����ɫ������󣬺�������ɫ�������ܸı�����ֵ����Ϊ��դ��ʱ����ȷ��������λ��
	float4 color : COLOR;
};