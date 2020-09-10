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

GameAPP::GameAPP(HINSTANCE hInstance) :D3DApp(hInstance), m_CBuffer()
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
	static float phi = 0.0f, theta = 0.0f,psi=0.0f;//X，Y, Z方向的旋转角
	phi += 0.001f;
	theta += 0.0015f;
	psi += 0.00030f;
	m_CBuffer.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta) * XMMatrixRotationZ(psi));
	// DYNAMIC更新常量缓冲区，让立方体转起来
	D3D11_MAPPED_SUBRESOURCE mappedData;//获取已经映射到缓冲区的内存

	//获取指向缓冲区中数据的指针并拒绝GPU对该缓冲区的访问
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//映射出来的内存我们可以通过memcpy_s函数来更新
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	//让指向资源的指针无效并重新启用GPU对该资源的访问权限
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);

}

void GameAPP::DrawScene()
{

	assert(m_pSwapChain);
	assert(m_pd3dImmediateContext);
	//在每一帧画面绘制的操作中，我们需要清理一遍渲染目标视图绑定的缓冲区
	static float blue[4] = { 0.0f,0.0f,0.0f,1.0f };//对后备缓冲区(R8G8B8A8)使用蓝色进行清空
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&blue));

	//同样在进行渲染之前，我们也要清理一遍深度/模板缓冲区
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present方法--前后备缓冲区交换并呈现
	//m_pd3dImmediateContext->DrawIndexed(54, 0, 0); //索引数目，起始索引位置，起始顶点位置
	m_pd3dImmediateContext->DrawIndexed(36, 0, 0);
	m_pd3dImmediateContext->DrawIndexed(18, 36, 7);
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
	HR(CreateShaderFromFile(L"HLSL\\Cube_VS.cso", L"HLSL\\Cube_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	//创建并绑定顶点输入布局
	HR(m_pd3d11Device->CreateInputLayout(VertexPosColor::inputlayout, ARRAYSIZE(VertexPosColor::inputlayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout.GetAddressOf()));
	//创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Cube_PS.cso", L"HLSL\\Cube_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));


	return true;

}

bool GameAPP::InitResource()
{
	// ******************
	// 设置立方体顶点
	//    5________ 6
	//    /|      /|
	//   /_|_____/ |
	//  1|4|_ _ 2|_|7
	//   | /     | /
	//   |/______|/
	//  0       3

	VertexPosColor CubeVertices[] =
	{
		/*
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
		*/
		// cube 8 points
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4{1.0f, 0.0f, 0.0f, 0.0f} },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4{0.0f, 1.0f, 0.0f, 0.0f} },
		{ XMFLOAT3(3.0f, 1.0f, -1.0f), XMFLOAT4{0.0f, 0.0f, 1.0f, 0.0f} },
		{ XMFLOAT3(3.0f, -1.0f, -1.0f), XMFLOAT4{1.0f, 1.0f, 0.0f, 0.0f} },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4{1.0f, 0.0f, 1.0f, 0.0f} },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4{0.0f, 1.0f, 1.0f, 0.0f} },
		{ XMFLOAT3(3.0f, 1.0f, 1.0f), XMFLOAT4{0.0f, 0.0f, 0.0f, 0.0f} },
		{ XMFLOAT3(3.0f, -1.0f, 1.0f), XMFLOAT4{1.0f, 1.0f, 1.0f, 0.0f} },

		// Rectangular pyramid 5 points
		{ XMFLOAT3(-3.0f, 0.0f, 0.0f), XMFLOAT4{1.0f, 0.0f, 0.0f, 0.0f} },
		{ XMFLOAT3(-2.0f, 0.0f, 1.0f), XMFLOAT4{1.0f, 1.0f, 0.0f, 0.0f} },
		{ XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4{1.0f, 0.0f, 1.0f, 0.0f} },
		{ XMFLOAT3(-2.0f, 0.0f, -1.0f), XMFLOAT4{0.0f, 1.0f, 1.0f, 0.0f} },
		{ XMFLOAT3(-2.0f, 1.0f, 0.0f), XMFLOAT4{0.0f, 1.0f, 0.0f, 0.0f} },
	};

	//设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof CubeVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	//新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = CubeVertices;
	HR(m_pd3d11Device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	//索引数组
	DWORD indices[] =
	{/*
		//正面
		0,1,2,
		2,3,0,
		//左面
		4,5,1,
		1,0,4,
		//上面
		1,5,6,
		6,2,1,
		//背面
		7,6,5,
		5,4,7,
		//右面
		3,2,6,
		6,7,3,
		//底面
		4,0,3,
		3,7,4,
		
		//Rectangular pyramid 四棱锥
		8, 12, 11,
		11, 12, 10,
		10, 12, 9,
		9, 12, 8,
		8, 11, 9, // 地面2四边形由两个三角形组成
		9, 11, 10 // 注意地面的顺序
		*/
		// cube
		0, 1, 2,
		2, 3, 0, // 正面
		4, 5, 1,
		1, 0, 4, // 左面
		1, 5, 6,
		6, 2, 1, // 上面
		3, 2, 6,
		6, 7, 3, // 右面
		7, 6, 5,
		5, 4, 7, // 背面
		3, 7, 4,
		4, 0, 3,  // 下面
		//Rectangular pyramid 四棱锥
		1, 5, 4,
		4, 5, 3,
		3, 5, 2,
		2, 5, 1,
		1, 4, 2, // 地面2四边形由两个三角形组成
		2, 4, 3  // 注意地面的顺序
	};

	//设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof indices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	//新建索引缓冲区
	InitData.pSysMem = indices;
	HR(m_pd3d11Device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	//输入装备阶段索引缓冲区设置
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	//设置常数缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建常量缓冲区，不使用初始数据
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.GetAddressOf()));

	//初始化常量缓冲区的值
	//其中view和projective矩阵需要预先进行一次转置以抵消HLSL列主矩阵的转置，至于world矩阵已经是单位矩阵就不需要了
	m_CBuffer.world = XMMatrixIdentity();//单位矩阵转置为自身
	//Builds a view matrix for a left-handed coordinate system using a camera position, a focal point, and an up direction.    
	m_CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH
	(
		XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));
	//根据视场建立左手透视投影矩阵
	//XMMATRIX XM_CALLCONV XMMatrixPerspectiveFovLH(
		//float FovAngleY,自上而下的视场角（以弧度为单位）。
		//float AspectRatio,视空间的宽高比X：Y
		//float NearZ,近平面（正）
		//float FarZ 远平面（负）
	//);
	m_CBuffer.projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI*0.6f, AspectRatio(), 1.0f, 1000.0f));//XM_PIDIV2：PI/2



	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);
	UINT offset = 0;
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//渲染管线输入装配阶段设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout.Get());
	//将着色器绑定到渲染管线，给渲染管线某一着色阶段设置对应的着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	//顶点着色阶段设置常量缓冲区供使用
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	D3D11SetDebugObjectName(m_pVertexInputLayout.Get(), "VertexPosColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "VertexShader");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "PixelShader");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffer.Get(), "ConstantBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

	return true;

}
