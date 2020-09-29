#include"Shader.hlsli"

float4 PS_3D(VertexPosHWNormalWTex verticesIn) : SV_Target
{
    //��׼������
    verticesIn.NormalW = normalize(verticesIn.NormalW);
    //��ɫ�㿴���۾�������
    float3 toEyeVec = normalize(EyePosW - verticesIn.PosW);

    //��ʼ�����룬�������
    float4 ambientOut, diffuseOut, specularOut; 
    float4 ambientIn, diffuseIn, specularIn; 
    ambientOut=diffuseOut=specularOut=ambientIn=diffuseIn=specularIn=float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    int i = 0;
    
    //���㷽���
    for (i = 0; i < NumDirectionalLight;i++)
    {        
        ComputeDirectionalLight(g_Matrial, g_DirectionalLight[i], verticesIn.NormalW, toEyeVec,
        ambientIn, diffuseIn, specularIn);
        ambientOut += ambientIn;
        diffuseOut += diffuseIn;
        specularOut += specularIn;
    }

    //������Դ
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
        
    
    //����������ɫ
    float4 TexColor = g_Tex.Sample(g_SamplerState, verticesIn.TEXTURE);
    float4 LitColor = TexColor * (ambientOut + diffuseOut) + specularOut;
    LitColor.a = g_Matrial.Diffuse.a * TexColor.a;
    
    return LitColor;

}