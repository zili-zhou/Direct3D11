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
		DirectionalLight directionalLight[10];
		PointLight pointLight[10];
		SpotLight spotLight[10];

		int NumDirectionalLight;
		int NumPointLight;
		int NumSpotLight;
		float pad;//16字节打包对齐

		DirectX::XMFLOAT4 eyePos;
	};

	enum class ShowMode {WoodCreate, FireCreate, FireAnimation};

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

	template<class VertexType>
	bool ResetMesh(const Geometry::MeshData<VertexType, DWORD>& meshData);//重新设置要使用的模型

private:

	ComPtr<ID2D1SolidColorBrush> m_pSolidColorBrush;      //创建单色笔刷
	ComPtr<IDWriteTextFormat> m_pTextFormat;              //文本格式

	ComPtr<ID3D11InputLayout> m_pVertexInputLayout2D;     //用于2D的顶点输入布局
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout3D;     //用于3D的顶点输入布局	
	ComPtr<ID3D11Buffer> m_pVertexBuffer;                //顶点缓冲区
	ComPtr<ID3D11Buffer>m_pIndexBuffer;                  //索引缓冲区
	ComPtr<ID3D11Buffer>m_pConstantBuffer[2];            //常量缓冲区
	UINT m_indexCount;                                   //绘制物体的索引大小
	int m_CurrFrame;                                       //当前播放到第几帧
	ShowMode m_CurrMode;                                   //当前显示模式

	//纹理并不能直接绑定到着色器中，需要为纹理创建对应的着色器资源视图才能够给着色器使用
	ComPtr<ID3D11ShaderResourceView> m_pWoodCreate;           //木盒纹理
	ComPtr<ID3D11ShaderResourceView> m_pFireCreate;            //火焰木盒
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_pFireAnim; //火焰纹理数组
	ComPtr<ID3D11SamplerState> m_pSamplerState;          //采样器状态

	ComPtr<ID3D11VertexShader> m_pVertexShader2D;          //用于2D的顶点着色器
	ComPtr<ID3D11VertexShader> m_pVertexShader3D;          //用于3D顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader2D;            //用于2D的像素着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader3D;            //用于3D的像素着色器

	VSConstantBuffer m_VSConstantBuffer;                 //用于修改VS的GPU常量缓冲区的变量
	PSConstantBuffer m_PSConstantBuffer;                 //用于修改PS的GPU常量缓冲区的变量

	//三种光源
	DirectionalLight m_DirLight;
	PointLight m_PointLight;
	SpotLight m_SpotLight;
	/*
	ComPtr<ID3D11RasterizerState> m_pRSWireframe;        //输出光栅化的绘制线框状态
	bool m_IsWireFrameMode;                              //当前是否绘制线框
	*/
};






#endif // !GAMEAPP_H
