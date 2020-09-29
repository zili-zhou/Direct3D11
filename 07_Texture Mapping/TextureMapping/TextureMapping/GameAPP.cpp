#include"GameAPP.h"
#include"D3DUtil.h"
#include"DXTrace.h"
#include<assert.h>

using namespace DirectX;


GameAPP::GameAPP(HINSTANCE hInstance) :D3DApp(hInstance),
	m_indexCount(), m_VSConstantBuffer(), m_PSConstantBuffer(), m_DirLight(), m_PointLight(), m_SpotLight(), m_CurrFrame(),m_CurrMode(ShowMode::WoodCreate)
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

	//初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	return true;
}

void GameAPP::OnResize()
{
	assert(m_pd2dFactory);
	assert(m_pdWriteFactory);

	// 释放D2D的相关资源
	m_pd2dRenderTarget.Reset();
	m_pSolidColorBrush.Reset();

	//D2D的相关资源需要在D3D相关资源释放前先行释放掉，然后在D3D重设后备缓冲区后重新创建D2D渲染目标
	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标

	//通过IDXGISwapChain::GetBuffer方法来获取后备缓冲区的IDXGISurface接口
	ComPtr<IDXGISurface> surface;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_pSolidColorBrush.GetAddressOf()));
		HR(m_pdWriteFactory->CreateTextFormat(L"微软雅黑", nullptr,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"zh-cn", m_pTextFormat.GetAddressOf()));
	}
	else
		assert(m_pd2dRenderTarget);
}

void GameAPP::UpdateScene(float dt)//dt是两帧之间的时间间隔
{
	Keyboard::State keyboardState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyboardState);

	//键盘切换模式
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D1) && m_CurrMode != ShowMode::WoodCreate)
	{
		//播放木箱
		//纹理并不能直接绑定到着色器中，需要为纹理创建对应的着色器资源视图才能够给着色器使用
		m_CurrMode = ShowMode::WoodCreate;
		auto meshData = Geometry::CreateBox<VertexPosNormalTexture,DWORD>();
		ResetMesh(meshData);
		m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());
		m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCreate.GetAddressOf());//起始槽索引，对应HLSL的register(t*);着色器资源视图数目;着色器资源视图数组
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D2) && m_CurrMode != ShowMode::FireCreate)
	{
		//播放木箱火焰
		//纹理并不能直接绑定到着色器中，需要为纹理创建对应的着色器资源视图才能够给着色器使用
		m_CurrMode = ShowMode::FireCreate;
		auto meshData = Geometry::CreateBox<VertexPosNormalTexture, DWORD>();
		ResetMesh(meshData);
		m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());
		m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pFireCreate.GetAddressOf());//起始槽索引，对应HLSL的register(t*);着色器资源视图数目;着色器资源视图数组
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D3) && m_CurrMode != ShowMode::FireAnimation)
	{
		m_CurrMode = ShowMode::FireAnimation;
		m_CurrFrame = 0;
		auto meshData = Geometry::Create2DShow<VertexPosTex, DWORD>();
		ResetMesh(meshData);
		m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout2D.Get());
		m_pd3dImmediateContext->VSSetShader(m_pVertexShader2D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShader(m_pPixelShader2D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pFireAnim[0].GetAddressOf());
	}

	if (m_CurrMode == ShowMode::WoodCreate|| m_CurrMode == ShowMode::FireCreate)
	{
		static float objPhi = 0.0f, objTheta = 0.0f;//物体 X Y方向转角
		objPhi += 0.0002f;
		objTheta += 0.0003f;
		XMMATRIX Matrix = XMMatrixRotationX(objPhi) * XMMatrixRotationY(objTheta);
		m_VSConstantBuffer.world = XMMatrixTranspose(Matrix);
		m_VSConstantBuffer.worldInvTranspose = XMMatrixTranspose(InverseTranspose(Matrix));

		// DYNAMIC更新常量缓冲区，让物体转起来
		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(m_VSConstantBuffer), &m_VSConstantBuffer, sizeof(m_VSConstantBuffer));
		m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);
	}
	else if (m_CurrMode == ShowMode::FireAnimation)
	{
		// 用于限制在1秒60帧
		static float totDeltaTime = 0;

		totDeltaTime += dt;
		if (totDeltaTime > 1.0f / 60)
		{
			totDeltaTime -= 1.0f / 60;
			m_CurrFrame = (m_CurrFrame + 1) % 120;
			m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pFireAnim[m_CurrFrame].GetAddressOf());
		}

	}

}

template<class VertexType>
bool GameAPP::ResetMesh(const Geometry::MeshData<VertexType, DWORD>& meshData)
{
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexType);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(m_pd3d11Device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexType);	// 跨越字节数
	UINT offset = 0;							// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);



	// 设置索引缓冲区描述
	m_indexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_indexCount * sizeof(DWORD);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = meshData.indexVec.data();
	HR(m_pd3d11Device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// 输入装配阶段的索引缓冲区设置
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);



	// 设置调试对象名
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

	return true;
}



void GameAPP::DrawScene()
{

	assert(m_pSwapChain);
	assert(m_pd3dImmediateContext);
	
	//绘制Direct3D部分

	//在每一帧画面绘制的操作中，我们需要清理一遍渲染目标视图绑定的缓冲区
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));

	//同样在进行渲染之前，我们也要清理一遍深度/模板缓冲区
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present方法--前后备缓冲区交换并呈现
	//绘制几何形体，索引数目，起始索引位置，起始顶点位置
	m_pd3dImmediateContext->DrawIndexed(m_indexCount, 0, 0);

	//绘制Direct2D部分
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring textStr = L"切换显示: 1-木箱(3D) 2-火焰木箱(3D) 3-火焰(2D)\n";
		m_pd2dRenderTarget->DrawTextW(textStr.c_str(), textStr.size(), m_pTextFormat.Get(), D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pSolidColorBrush.Get());
		HR(m_pd2dRenderTarget->EndDraw());
	}


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

	//创建2D顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\VS_2D.cso", L"HLSL\\VS_2D.hlsl", "VS_2D", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader2D.GetAddressOf()));
	//创建并绑定顶点输入布局
	HR(m_pd3d11Device->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout2D.GetAddressOf()));

	//创建3D顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\VS_3D.cso", L"HLSL\\VS_3D.hlsl", "VS_3D", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader3D.GetAddressOf()));
	//创建并绑定顶点输入布局
	HR(m_pd3d11Device->CreateInputLayout(VertexPosNormalTexture::inputLayout, ARRAYSIZE(VertexPosNormalTexture::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout3D.GetAddressOf()));

	//创建2D像素着色器
	HR(CreateShaderFromFile(L"HLSL\\PS_2D.cso", L"HLSL\\PS_2D.hlsl", "PS_2D", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader2D.GetAddressOf()));

	//创建3D像素着色器
	HR(CreateShaderFromFile(L"HLSL\\PS_3D.cso", L"HLSL\\PS_3D.hlsl", "PS_3D", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader3D.GetAddressOf()));

	return true;

}

bool GameAPP::InitResource()
{

	//初始化网格模型
	auto meshData = Geometry::CreateBox<VertexPosNormalTexture, DWORD>();

	ResetMesh(meshData);

	//分别设置用于VS,PS的常数缓冲区描述
	//顶点，索引缓冲区放在ResetMesh中
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[0].GetAddressOf()));

	cbd.ByteWidth = sizeof(PSConstantBuffer);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[1].GetAddressOf()));

	//初始化用于VS的常量缓冲区的值

	//其中view和projective矩阵需要预先进行一次转置以抵消HLSL列主矩阵的转置，至于world矩阵已经是单位矩阵就不需要了
	m_VSConstantBuffer.world = XMMatrixIdentity();//单位矩阵转置为自身
	//Builds a view matrix for a left-handed coordinate system using a camera position, a focal point, and an up direction.    
	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH
	(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
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
	m_VSConstantBuffer.projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI*0.6f, AspectRatio(), 1.0f, 1000.0f));//XM_PIDIV2：PI/2
	m_VSConstantBuffer.worldInvTranspose = XMMatrixIdentity();

	// 更新VS常量缓冲区资源
	D3D11_MAPPED_SUBRESOURCE mappedData1;//获取已经映射到缓冲区的内存
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData1));
	memcpy_s(mappedData1.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);


	//初始化用于PS的常量缓冲区的值
	
	// 初始化用于PS的常量缓冲区的值
	// 这里只使用一盏点光来演示
	m_PSConstantBuffer.pointLight[0].position = XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_PSConstantBuffer.pointLight[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PSConstantBuffer.pointLight[0].diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PSConstantBuffer.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PSConstantBuffer.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PSConstantBuffer.pointLight[0].range = 25.0f;
	m_PSConstantBuffer.NumDirectionalLight = 0;
	m_PSConstantBuffer.NumPointLight = 1;
	m_PSConstantBuffer.NumSpotLight = 0;
	
	m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PSConstantBuffer.material.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PSConstantBuffer.material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_PSConstantBuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);

	// 更新PS常量缓冲区资源

	D3D11_MAPPED_SUBRESOURCE mappedData2;//获取已经映射到缓冲区的内存
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData2));
	memcpy_s(mappedData2.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);

	// 初始化纹理和采样器状态

	//初始化木箱纹理
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\WoodCrate.dds", nullptr, m_pWoodCreate.GetAddressOf()));

	//初始化火焰木箱
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\flare.dds", nullptr, m_pFireCreate.GetAddressOf()));

	//初始化火焰纹理
	WCHAR strFile[40];
	m_pFireAnim.resize(120);
	for (int i = 1; i <= 120; i++)
	{
		wsprintf(strFile, L"..\\Texture\\FireAnim\\Fire%03d.bmp", i);
		HR(CreateWICTextureFromFile(m_pd3d11Device.Get(), strFile, nullptr, m_pFireAnim[static_cast<size_t>(i) - 1].GetAddressOf()));
	}

	//初始化采样器状态
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(m_pd3d11Device->CreateSamplerState(&samplerDesc, m_pSamplerState.GetAddressOf()));

	// 给渲染管线各个阶段绑定好所需资源

	//渲染管线输入装配阶段设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());
	//默认绑定3D着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
	// VS常量缓冲区对应HLSL寄存于b0的常量缓冲区,PS常量缓冲区对应HLSL寄存于b1的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer[0].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());

	// 像素着色阶段设置好采样器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCreate.GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);

	//设置调试对象名
	D3D11SetDebugObjectName(m_pVertexInputLayout3D.Get(), "VertexPosNormalColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexInputLayout2D.Get(), "VertexPosTextureInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader2D.Get(), "VertexShader2D");
	D3D11SetDebugObjectName(m_pVertexShader3D.Get(), "VertexShader3D");
	D3D11SetDebugObjectName(m_pPixelShader2D.Get(), "PixelShader2D");
	D3D11SetDebugObjectName(m_pPixelShader3D.Get(), "PixelShader3D");
	D3D11SetDebugObjectName(m_pConstantBuffer[0].Get(), "VSConstantBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffer[1].Get(), "PSConstantBuffer");
	D3D11SetDebugObjectName(m_pSamplerState.Get(), "SSLinearWrap");

	return true;

}
