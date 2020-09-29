#include"Shader.hlsli"

float4 PS_3D(VertexPosHWNormalWTex verticesIn) : SV_Target
{
    //标准化法线
    verticesIn.NormalW = normalize(verticesIn.NormalW);
    //着色点看向眼睛的向量
    float3 toEyeVec = normalize(EyePosW - verticesIn.PosW);

    //初始化输入，输出光线
    float4 ambientOut, diffuseOut, specularOut; 
    float4 ambientIn, diffuseIn, specularIn; 
    ambientOut=diffuseOut=specularOut=ambientIn=diffuseIn=specularIn=float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    int i = 0;
    
    //计算方向光
    for (i = 0; i < NumDirectionalLight;i++)
    {        
        ComputeDirectionalLight(g_Matrial, g_DirectionalLight[i], verticesIn.NormalW, toEyeVec,
        ambientIn, diffuseIn, specularIn);
        ambientOut += ambientIn;
        diffuseOut += diffuseIn;
        specularOut += specularIn;
    }

    //计算点光源
    for (i = 0; i < NumPointLight; i++)
    {
        ComputePointLight(g_Matrial, g_PointLight[i],verticesIn.PosW, verticesIn.NormalW, toEyeVec,
        ambientIn, diffuseIn, specularIn);
        ambientOut += ambientIn;
        diffuseOut += diffuseIn;
        specularOut += specularIn;
    }

    for (i = 0; i < NumSpotLight;i++)
    {
        ComputeSpotLight(g_Matrial, g_SpotLight[i], verticesIn.PosW, verticesIn.NormalW, toEyeVec,
        ambientIn, diffuseIn, specularIn);
        ambientOut += ambientIn;
        diffuseOut += diffuseIn;
        specularOut += specularIn;
    }
        
    
    //计算像素着色
    float4 TexColor = g_Tex.Sample(g_SamplerState, verticesIn.TEXTURE);
    float4 LitColor = TexColor * (ambientOut + diffuseOut) + specularOut;
    LitColor.a = g_Matrial.Diffuse.a * TexColor.a;
    
    return LitColor;

}