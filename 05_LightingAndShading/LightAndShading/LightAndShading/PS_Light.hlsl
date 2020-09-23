#include"Light.hlsli"

float4 PS(VertexOutput verticesIn) : SV_Target
{
    //��׼������
    verticesIn.NormalW = normalize(verticesIn.NormalW);
    //��ɫ�㿴���۾�������
    float3 toEyeVec = normalize(EyePosW - verticesIn.PosW);

    //��ʼ�����룬�������
    float4 ambientOut, diffuseOut, specularOut; 
    float4 ambientIn, diffuseIn, specularIn; 
    ambientOut=diffuseOut=specularOut=ambientIn=diffuseIn=specularIn=float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //���㷽���
    ComputeDirectionalLight(g_Matrial, g_DirectionalLight, verticesIn.NormalW, toEyeVec,
    ambientIn, diffuseIn, specularIn);
    ambientOut += ambientIn;
    diffuseOut += diffuseIn;
    specularOut += specularIn;
    //������Դ
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
    
    //����������ɫ
    float4 LitColor = verticesIn.Color * (ambientOut + diffuseOut) + specularOut;
    LitColor.a = g_Matrial.Diffuse.a * verticesIn.Color.a;
    
    return LitColor;

}