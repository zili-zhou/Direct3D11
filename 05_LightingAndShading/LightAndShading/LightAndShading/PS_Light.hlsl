#include"Light.hlsli"

float4 PS(VertexOutput verticesIn) : SV_Target
{
    //标准化法线
    verticesIn.NormalW = normalize(verticesIn.NormalW);
    //着色点看向眼睛的向量
    float3 toEyeVec = normalize(EyePosW - verticesIn.PosW);

    //初始化输入，输出光线
    float4 ambientOut, diffuseOut, specularOut; 
    float4 ambientIn, diffuseIn, specularIn; 
    ambientOut=diffuseOut=specularOut=ambientIn=diffuseIn=specularIn=float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //计算方向光
    ComputeDirectionalLight(g_Matrial, g_DirectionalLight, verticesIn.NormalW, toEyeVec,
    ambientIn, diffuseIn, specularIn);
    ambientOut += ambientIn;
    diffuseOut += diffuseIn;
    specularOut += specularIn;
    //计算点光源
    ComputePointLight(g_Matrial, g_PointLight,verticesIn.PosW, verticesIn.NormalW, toEyeVec,
    ambientIn, diffuseIn, specularIn);
    ambientOut += ambientIn;
    diffuseOut += diffuseIn;
    specularOut += specularIn;
    
    ComputeSpotLight(g_Matrial, g_SpotLight,verticesIn.PosW, verticesIn.NormalW, toEyeVec,
    ambientIn, diffuseIn, specularIn);
    ambientOut += ambientIn;
    diffuseOut += diffuseIn;
    specularOut += specularIn;
    
    //计算像素着色
    float4 LitColor = verticesIn.Color * (ambientOut + diffuseOut) + specularOut;
    LitColor.a = g_Matrial.Diffuse.a * verticesIn.Color.a;
    
    return LitColor;

}