// ����������
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;//w�Ǿ��淴��ϵ��SpecPower
    float4 Reflect;
};

//�����
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı�����������HLSL��������
};

//���Դ
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att; //��˥��ϵ����Attenuation coefficient
    float pad; // �����һ�����������ʹ�øýṹ���С����16�ı�����������HLSL��������
};

//�۹�Դ
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Spot;//��Ļ�۳̶�
    
    float3 Att;
    float Pad;
    
};


//���㷽�����
void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toViewPos, out float4 ambient, out float4 diffuse, out float4 specular)
//������ʣ�����⣬��ɫ�����ķ��ߣ������۾����������ɫ�㷢���Ļ����⣬�������;��淴���
{
    
    //��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 LightVec = -L.Direction;//��ɫ������ⷽ�������䷽���෴
    
    ambient = mat.Ambient * L.Ambient;//��ӻ�����
    
    float DiffuseFactor = dot(normal, LightVec);//���������ɫ�㷨�߼н�
    
    if (DiffuseFactor>0)
    {
        float3 ReflectVec = reflect(LightVec, normal); //�����:2*DiffuseFactor*normal-LightVec
        float SpecularFactor = pow(max(dot(ReflectVec, toViewPos), 0), mat.Specular.w);
        
        diffuse = DiffuseFactor * mat.Diffuse * L.Diffuse;
        specular = SpecularFactor * mat.Specular * L.Specular;
    } 
}

//������Դ����
void ComputePointLight(Material mat, PointLight L, float3 position , float3 normal, float3 toViewPos, out float4 ambient, out float4 diffuse, out float4 specular)
//������ʣ�����⣬��ɫ��λ�ã�����ķ��ߣ������۾����������ɫ�㷢���Ļ����⣬�������;��淴���
{
    
    //��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 LightVec = L.Position-position; //��ɫ������ⷽ�������䷽���෴
    
    float distance = length(LightVec);//���Դ����ɫ�����
    
    if (distance > L.Range)//���嵽���Դ���볬�����շ�Χ��������ɫ
        return;
    
    LightVec /= distance; //��׼�������
    
    
    ambient = mat.Ambient * L.Ambient; //��ӻ�����
    
    float DiffuseFactor = dot(normal, LightVec); //���������ɫ�㷨�߼н�
    
    if (DiffuseFactor > 0)
    {
        float3 ReflectVec = reflect(LightVec, normal); //�����:2*DiffuseFactor*normal-LightVec
        float SpecularFactor = pow(max(dot(ReflectVec, toViewPos), 0), mat.Specular.w);
        
        diffuse = DiffuseFactor * mat.Diffuse * L.Diffuse;
        specular = SpecularFactor * mat.Specular * L.Specular;
    }
    
    //��˥��
    float att = 1.0f / dot(L.Att , float3(1.0f, distance, distance * distance));
    diffuse *= att;
    specular *= att;
}


//����۹��Դ����
void ComputeSpotLight(Material mat, SpotLight L, float3 position, float3 normal, float3 toViewPos, out float4 ambient, out float4 diffuse, out float4 specular)
//������ʣ�����⣬��ɫ��λ�ã�����ķ��ߣ������۾����������ɫ�㷢���Ļ����⣬�������;��淴���
{
    
    //��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 LightVec = L.Position - position; //��ɫ������ⷽ�������䷽���෴
    
    float distance = length(LightVec); //���Դ����ɫ�����
    
    if (distance > L.Range)//���嵽���Դ���볬�����շ�Χ��������ɫ
        return;
    
    LightVec /= distance; //��׼�������
    
    
    ambient = mat.Ambient * L.Ambient; //��ӻ�����
    
    float DiffuseFactor = dot(normal, LightVec); //���������ɫ�㷨�߼н�
    
    if (DiffuseFactor > 0)
    {
        float3 ReflectVec = reflect(LightVec, normal); //�����:2*DiffuseFactor*normal-LightVec
        float SpecularFactor = pow(max(dot(ReflectVec, toViewPos), 0), mat.Specular.w);
        
        diffuse = DiffuseFactor * mat.Diffuse * L.Diffuse;
        specular = SpecularFactor * mat.Specular * L.Specular;
    }
    
    //����ϵ����˥��
    float spot = pow(max(dot(LightVec, L.Direction), 0), L.Spot);
    float att = spot / dot(L.Att, float3(1.0f, distance, distance * distance));
    
    ambient *= spot;
    diffuse *= att;
    specular *= att;
}