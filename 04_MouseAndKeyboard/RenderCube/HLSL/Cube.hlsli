cbuffer ConstantBuffer : register(b0) //常量缓冲区的变量类似于C++这边的全局常量,cbuffer常量缓冲区的类型，register(b0)常量缓冲区位于寄存器索引为0的缓冲区
{
    matrix g_World; //matrix可以用float4x4替代。不加row_major的情况下，矩阵默认为列主矩阵
    matrix g_View; //可以在前面添加row_major表示行主矩阵
    matrix g_Proj; //需要在C++代码端预先将矩阵进行转置
}

struct VertexIn
{
    float3 pos : POSISION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 posH : SV_Position; //说明该顶点的位置在从顶点着色器输出后，后续的着色器都不能改变它的值，作为光栅化时最终确定的像素位置
    float4 color : COLOR;
};