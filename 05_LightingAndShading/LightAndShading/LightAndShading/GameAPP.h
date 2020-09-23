#ifndef GAMEAPP_H
#define GAMEAPP_H
#include"D3DApp.h"
#include"DXTrace.h"
#include"Geometry.h"
#include"LightType.h"

class GameAPP :public D3DApp
{
public:
	struct VSConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX worldInvTranspose;
	};
		
	struct PSConstantBuffer
	{
		Material material;
		DirectionalLight directionalLight;
		PointLight pointLight;
		SpotLight spotLight;
		DirectX::XMFLOAT4 eyePos;
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
	bool ResetMesh(const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData);//重新设置要使用的模型

private:
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout;      //顶点输入布局	
	ComPtr<ID3D11Buffer> m_pVertexBuffer;                //顶点缓冲区
	ComPtr<ID3D11Buffer>m_pIndexBuffer;                  //索引缓冲区
	ComPtr<ID3D11Buffer>m_pConstantBuffer[2];            //常数缓冲区
	UINT m_indexCount;                                   //绘制物体的索引大小


	ComPtr<ID3D11VertexShader> m_pVertexShader;          //顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;            //像素着色器
	VSConstantBuffer m_VSConstantBuffer;                 //用于修改VS的GPU常量缓冲区的变量
	PSConstantBuffer m_PSConstantBuffer;                 //用于修改PS的GPU常量缓冲区的变量

	//三种光源
	DirectionalLight m_DirLight;
	PointLight m_PointLight;
	SpotLight m_SpotLight;

	ComPtr<ID3D11RasterizerState> m_pRSWireframe;        //输出光栅化的绘制线框状态
	bool m_IsWireFrameMode;                              //当前是否绘制线框

};






#endif // !GAMEAPP_H
