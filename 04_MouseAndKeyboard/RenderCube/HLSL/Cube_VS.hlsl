#include"Cube.hlsli"

//C++����˽���ת�ã�HLSL��ʹ��matrix(���������) ��mul�����������������(������)��
//��������(������ X ���������)����C++��Ҫ����һ�ξ���ת�ã�HLSL�ڲ�������ת�� 
// mul ���Ǿ���˷�, �����*Ҫ���������Ϊ��������ȵ��������󣬽��ΪCij = Aij * Bij

//������ɫ��
VertexOut VS(VertexIn verticesInput)
{
    VertexOut verticesOutput;
    verticesOutput.posH = mul(float4(verticesInput.pos, 1.0f), g_World);
    verticesOutput.posH = mul(verticesOutput.posH, g_View);
    verticesOutput.posH = mul(verticesOutput.posH, g_Proj);
    verticesOutput.color = verticesInput.color;
    return verticesOutput;
}