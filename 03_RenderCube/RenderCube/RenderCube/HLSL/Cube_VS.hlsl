#include"Cube.hlsli"

//C++代码端进行转置，HLSL中使用matrix(列主序矩阵) ，mul函数让向量放在左边(行向量)，
//这样就是(行向量 X 列主序矩阵)，但C++需要进行一次矩阵转置，HLSL内部不产生转置 
// mul 才是矩阵乘法, 运算符*要求操作对象为行列数相等的两个矩阵，结果为Cij = Aij * Bij

//顶点着色器
VertexOut VS(VertexIn verticesInput)
{
    VertexOut verticesOutput;
    verticesOutput.posH = mul(float4(verticesInput.pos, 1.0f), g_World);
    verticesOutput.posH = mul(verticesOutput.posH, g_View);
    verticesOutput.posH = mul(verticesOutput.posH, g_Proj);
    verticesOutput.color = verticesInput.color;
    return verticesOutput;
}