cbuffer ConstantBuffer : register(b0) //�����������ı���������C++��ߵ�ȫ�ֳ���,cbuffer���������������ͣ�register(b0)����������λ�ڼĴ�������Ϊ0�Ļ�����
{
    matrix g_World; //matrix������float4x4���������row_major������£�����Ĭ��Ϊ��������
    matrix g_View; //������ǰ�����row_major��ʾ��������
    matrix g_Proj; //��Ҫ��C++�����Ԥ�Ƚ��������ת��
}

struct VertexIn
{
    float3 pos : POSISION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 posH : SV_Position; //˵���ö����λ���ڴӶ�����ɫ������󣬺�������ɫ�������ܸı�����ֵ����Ϊ��դ��ʱ����ȷ��������λ��
    float4 color : COLOR;
};