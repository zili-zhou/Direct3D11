#include"GameAPP.h"
#include"D3DUtil.h"
#include"DXTrace.h"
#include<assert.h>

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC GameAPP::VertexPosColor::inputlayout[2] = 
{
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
};

GameAPP::GameAPP(HINSTANCE hInstance):D3DApp(hInstance)
{

}

GameAPP::~GameAPP()
{}

bool GameAPP::Init()
{
	if (!D3DApp::Init())
		return false;
	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;
	return true;
}

void GameAPP::OnResize()
{
	D3DApp::OnResize();
}

void GameAPP::UpdateScene(float dt)
{

}

void GameAPP::DrawScene()
{
	
	assert(m_pSwapChain);
	assert(m_pd3dImmediateContext);
	//在每一帧画面绘制的操作中，我们需要清理一遍渲染目标视图绑定的缓冲区
	static float blue[4] = { 0.0f,0.0f,0.0f,1.0f };//对后备缓冲区(R8G8B8A8)使用蓝色进行清空
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);

	//同样在进行渲染之前，我们也要清理一遍深度/模板缓冲区
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present方法--前后备缓冲区交换并呈现
	m_pd3dImmediateContext->Draw(3, 0);//需要绘制的顶点数目,起始顶点索引
	HR(m_pSwapChain->Present(0, 0));

}

bool GameAPP::InitEffect()
{
	
	ComPtr<ID3DBlob> blob;
	//创建顶点着色器
	// ------------------------------
	// CreateShaderFromFile函数
	// ------------------------------
	// [In]csoFileNameInOut 编译好的着色器二进制文件(.cso)，若有指定则优先寻找该文件并读取
	// [In]hlslFileName     着色器代码，若未找到着色器二进制文件则编译着色器代码
	// [In]entryPoint       入口点(指定开始的函数)
	// [In]shaderModel      着色器模型，格式为"*s_5_0"，*可以为c,d,g,h,p,v之一
	// [Out]ppBlobOut       输出着色器二进制信息
	HR(CreateShaderFromFile(L"HLSL\\Triangle_VS.cso", L"HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	//创建并绑定顶点输入布局
	HR(m_pd3d11Device->CreateInputLayout(VertexPosColor::inputlayout, ARRAYSIZE(VertexPosColor::inputlayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout.GetAddressOf()));

	//创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_PS.cso", L"HLSL\\Triangle_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
	
}

bool GameAPP::InitResource()
{
	
	// 设置三角形顶点
	// 注意三个顶点的给出顺序应当按顺时针排布
	VertexPosColor TriVertices[] =
	{
		//Triangle
		{ XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }

	};

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;//vertex buffer description
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.ByteWidth = sizeof TriVertices;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	//新建顶点缓冲区初始化的数据
	D3D11_SUBRESOURCE_DATA IniData;
	ZeroMemory(&IniData, sizeof(IniData));
	IniData.pSysMem = TriVertices;//// 用于初始化的数据
	HR(m_pd3d11Device->CreateBuffer(&vbd, &IniData, m_pVertexBuffer.GetAddressOf()));


	// 给渲染管线各个阶段绑定好所需资源


	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor); //跨越字节数
	UINT offset = 0;   //起始偏移量
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	//渲染管线输入装配阶段设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//图元类型
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout.Get());

	//将着色器绑定到渲染管线，给渲染管线某一着色阶段设置对应的着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	//设置调试对象名
	D3D11SetDebugObjectName(m_pVertexInputLayout.Get(), "VertexPosColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "VertexShader");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "PixelShader");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");


	return true;
	
}