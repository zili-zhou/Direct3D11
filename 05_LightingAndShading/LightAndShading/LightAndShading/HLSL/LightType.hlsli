// 物体表面材质
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;//w是镜面反射系数SpecPower
    float4 Reflect;
};

//方向光
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于在HLSL设置数组
};

//点光源
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att; //光衰减系数，Attenuation coefficient
    float pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于在HLSL设置数组
};

//聚光源
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Spot;//光的汇聚程度
    
    float3 Att;
    float Pad;
    
};


//计算方向光照
void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toViewPos, out float4 ambient, out float4 diffuse, out float4 specular)
//输入材质，方向光，着色点表面的法线，看向眼睛方向，输出着色点发出的环境光，漫反射光和镜面反射光
{
    
    //初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 LightVec = -L.Direction;//着色点入射光方向与照射方向相反
    
    ambient = mat.Ambient * L.Ambient;//添加环境光
    
    float DiffuseFactor = dot(normal, LightVec);//入射光与着色点法线夹角
    
    if (DiffuseFactor>0)
    {
        float3 ReflectVec = reflect(LightVec, normal); //反射光:2*DiffuseFactor*normal-LightVec
        float SpecularFactor = pow(max(dot(ReflectVec, toViewPos), 0), mat.Specular.w);
        
        diffuse = DiffuseFactor * mat.Diffuse * L.Diffuse;
        specular = SpecularFactor * mat.Specular * L.Specular;
    } 
}

//计算点光源光照
void ComputePointLight(Material mat, PointLight L, float3 position , float3 normal, float3 toViewPos, out float4 ambient, out float4 diffuse, out float4 specular)
//输入材质，方向光，着色点位置，表面的法线，看向眼睛方向，输出着色点发出的环境光，漫反射光和镜面反射光
{
    
    //初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 LightVec = L.Position-position; //着色点入射光方向与照射方向相反
    
    float distance = length(LightVec);//点光源到着色点距离
    
    if (distance > L.Range)//物体到点光源距离超过光照范围不计算着色
        return;
    
    LightVec /= distance; //标准化入射光
    
    
    ambient = mat.Ambient * L.Ambient; //添加环境光
    
    float DiffuseFactor = dot(normal, LightVec); //入射光与着色点法线夹角
    
    if (DiffuseFactor > 0)
    {
        float3 ReflectVec = reflect(LightVec, normal); //反射光:2*DiffuseFactor*normal-LightVec
        float SpecularFactor = pow(max(dot(ReflectVec, toViewPos), 0), mat.Specular.w);
        
        diffuse = DiffuseFactor * mat.Diffuse * L.Diffuse;
        specular = SpecularFactor * mat.Specular * L.Specular;
    }
    
    //光衰减
    float att = 1.0f / dot(L.Att , float3(1.0f, distance, distance * distance));
    diffuse *= att;
    specular *= att;
}


//计算聚光光源光照
void ComputeSpotLight(Material mat, SpotLight L, float3 position, float3 normal, float3 toViewPos, out float4 ambient, out float4 diffuse, out float4 specular)
//输入材质，方向光，着色点位置，表面的法线，看向眼睛方向，输出着色点发出的环境光，漫反射光和镜面反射光
{
    
    //初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 LightVec = L.Position - position; //着色点入射光方向与照射方向相反
    
    float distance = length(LightVec); //点光源到着色点距离
    
    if (distance > L.Range)//物体到点光源距离超过光照范围不计算着色
        return;
    
    LightVec /= distance; //标准化入射光
    
    
    ambient = mat.Ambient * L.Ambient; //添加环境光
    
    float DiffuseFactor = dot(normal, LightVec); //入射光与着色点法线夹角
    
    if (DiffuseFactor > 0)
    {
        float3 ReflectVec = reflect(LightVec, normal); //反射光:2*DiffuseFactor*normal-LightVec
        float SpecularFactor = pow(max(dot(ReflectVec, toViewPos), 0), mat.Specular.w);
        
        diffuse = DiffuseFactor * mat.Diffuse * L.Diffuse;
        specular = SpecularFactor * mat.Specular * L.Specular;
    }
    
    //光汇聚系数和衰减
    float spot = pow(max(dot(LightVec, L.Direction), 0), L.Spot);
    float att = spot / dot(L.Att, float3(1.0f, distance, distance * distance));
    
    ambient *= spot;
    diffuse *= att;
    specular *= att;
}