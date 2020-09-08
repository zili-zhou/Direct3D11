#include"Cube.hlsli"

//像素着色器
float4 PS(VertexOut verticesInput) : SV_Target //说明输出的颜色值将会直接保存到渲染目标视图的后备缓冲区对应位置
{
    return verticesInput.color;
}