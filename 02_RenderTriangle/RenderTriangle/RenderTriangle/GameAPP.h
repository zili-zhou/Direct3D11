#ifndef GAMEAPP_H
#define GAMEAPP_H
#include"D3DApp.h"
#include"DXTrace.h"

class GameAPP :public D3DApp
{
public:
	struct VertexPosColor//能够建立C++结构体与HLSL结构体(hlsli)的对应关系，需要使用ID3D11InputLayout输入布局来描述每一个成员的用途、语义、大小等信息
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		static const D3D11_INPUT_ELEMENT_DESC inputlayout[2];//静态全局变量，描述待传入结构体中每个成员的具体信息
	};

public:
	GameAPP(HINSTANCE hInstance);
	~GameAPP();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
private:
	bool InitEffect();
	bool InitResource();

private:
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout;//顶点输入布局
	ComPtr<ID3D11VertexShader> m_pVertexShader;//顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;//像素着色器
	ComPtr<ID3D11Buffer> m_pVertexBuffer;//顶点缓冲区

};






#endif // !GAMEAPP_H
