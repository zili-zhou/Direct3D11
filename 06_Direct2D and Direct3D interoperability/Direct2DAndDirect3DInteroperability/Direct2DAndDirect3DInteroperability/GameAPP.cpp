#include"GameAPP.h"
#include"D3DUtil.h"
#include"DXTrace.h"
#include<assert.h>

using namespace DirectX;


GameAPP::GameAPP(HINSTANCE hInstance) :D3DApp(hInstance),
	m_indexCount(), m_VSConstantBuffer(), m_PSConstantBuffer(), m_DirLight(), m_PointLight(), m_SpotLight(), m_IsWireFrameMode(false)
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
	static float objPhi = 0.0f, objTheta = 0.0f;//物体 X Y方向转角
	objPhi += 0.0004f; 
	objTheta += 0.0006f;

	XMMATRIX Matrix = XMMatrixRotationX(objPhi) * XMMatrixRotationY(objTheta);
	m_VSConstantBuffer.world = XMMatrixTranspose(Matrix);
	m_VSConstantBuffer.worldInvTranspose = XMMatrixTranspose(InverseTranspose(Matrix));



	//获取鼠标初始，最后状态
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();

	// 更新鼠标按钮状态跟踪器，仅当鼠标按住的情况下才进行移动
	m_MouseTracker.Update(mouseState);
	

	if (mouseState.leftButton == TRUE && m_MouseTracker.leftButton == m_MouseTracker.HELD)
	{
		objTheta -= (mouseState.x - lastMouseState.x) * 0.01f;
		objPhi -= (mouseState.y - lastMouseState.y) * 0.01f;
	}
	if (mouseState.rightButton == TRUE)
	{
		objPhi = 0.0f;
		objTheta = 0.0f;

	}

	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH
	(
		XMVectorSet(1.0f, 0.0f, -5.0f + mouseState.scrollWheelValue * 0.01f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));

	//键盘切换灯光类型
	Keyboard::State keyboardState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyboardState);
	
	if (keyboardState.IsKeyDown(Keyboard::Keys::W))
		objPhi += dt * 2;
	if (keyboardState.IsKeyDown(Keyboard::Keys::S))
		objPhi -= dt * 2;
	if (keyboardState.IsKeyDown(Keyboard::Keys::A))
		objTheta += dt * 2;
	if (keyboardState.IsKeyDown(Keyboard::Keys::D))
		objTheta -= dt * 2;

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D1))
	{		
		m_PSConstantBuffer.directionalLight = m_DirLight;
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = SpotLight();
	}
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D2))
	{		
		m_PSConstantBuffer.directionalLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = m_PointLight;
		m_PSConstantBuffer.spotLight = SpotLight();
	}
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D3))
	{
		m_PSConstantBuffer.directionalLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = m_SpotLight;
	}
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D4))
	{
		m_PSConstantBuffer.directionalLight = m_DirLight;
		m_PSConstantBuffer.pointLight = m_PointLight;
		m_PSConstantBuffer.spotLight = m_SpotLight;
	}

	// 键盘切换模型类型


	
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::NumPad1))
	{
		const Geometry::MeshData<VertexPosNormalColor,DWORD>& meshData = Geometry::CreateBox<VertexPosNormalColor, DWORD>();
		ResetMesh(meshData);
	}
	
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::NumPad2))
	{
		const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData = Geometry::CreateSphere<VertexPosNormalColor, DWORD>();
		ResetMesh(meshData);
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::NumPad3))
	{
		const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData = Geometry::CreateCone<VertexPosNormalColor, DWORD>();
		ResetMesh(meshData);
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::NumPad4))
	{
		const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData = Geometry::CreateCylinder<VertexPosNormalColor, DWORD>();
		ResetMesh(meshData);
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::Tab))
	{
		m_IsWireFrameMode = !m_IsWireFrameMode;
		m_pd3dImmediateContext->RSSetState(m_IsWireFrameMode == true ? m_pRSWireframe.Get() : nullptr);//设置光栅化状态，若为nullptr，则使用默认光栅化状态
	}
	
	// DYNAMIC更新常量缓冲区，让物体转起来
	D3D11_MAPPED_SUBRESOURCE mappedData;//获取已经映射到缓冲区的内存

	//获取指向缓冲区中数据的指针并拒绝GPU对该缓冲区的访问
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//映射出来的内存我们可以通过memcpy_s函数来更新
	memcpy_s(mappedData.pData, sizeof(m_VSConstantBuffer), &m_VSConstantBuffer, sizeof(m_VSConstantBuffer));
	//让指向资源的指针无效并重新启用GPU对该资源的访问权限
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);

	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(m_PSConstantBuffer), &m_PSConstantBuffer, sizeof(m_PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);	
}

bool GameAPP::ResetMesh(const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData)
{
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexPosNormalColor);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(m_pd3d11Device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosNormalColor);	// 跨越字节数
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
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::DarkGray));

	//同样在进行渲染之前，我们也要清理一遍深度/模板缓冲区
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present方法--前后备缓冲区交换并呈现
	//绘制几何形体，索引数目，起始索引位置，起始顶点位置
	m_pd3dImmediateContext->DrawIndexed(m_indexCount, 0, 0);

	//绘制Direct2D部分
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring textStr = L"切换灯光类型: 1-平行光 2-点光 3-聚光灯\n"
			L"切换模型: NUM1-立方体 NUM2-球体 NUM3-圆锥体 NUM4-圆柱体\n"
			L"TAB-切换模式 当前模式: ";
		if (m_IsWireFrameMode)
			textStr += L"线框模式";
		else
			textStr += L"面模式";
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
	HR(CreateShaderFromFile(L"HLSL\\VS_Light.cso", L"HLSL\\VS_Light.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	//创建并绑定顶点输入布局
	HR(m_pd3d11Device->CreateInputLayout(VertexPosNormalColor::inputLayout, ARRAYSIZE(VertexPosNormalColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout.GetAddressOf()));
	//创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\PS_Light.cso", L"HLSL\\PS_Light.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));


	return true;

}

bool GameAPP::InitResource()
{

	//初始化网格模型
	auto meshData = Geometry::CreateBox<VertexPosNormalColor, DWORD>();

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
	
	//初始化默认光照
	//方向光
	m_DirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.direction = XMFLOAT3(-0.577f, -0.577f, 0.577f);
	
	// 点光
	m_PointLight.position = XMFLOAT3(0.0f, 1.0f, -10.0f);
	m_PointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PointLight.att = XMFLOAT3(0.0f, 0.1f, 0.004f);
	m_PointLight.range = 25.0f;

	//聚光

	m_SpotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_SpotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_SpotLight.spot = 12.0f;
	m_SpotLight.position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	m_SpotLight.range = 10000.0f;
	m_SpotLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	
	
	
	m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSConstantBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 5.0f);

	//使用默认方向光
	m_PSConstantBuffer.directionalLight = m_DirLight;

	m_PSConstantBuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);

	// 更新PS常量缓冲区资源

	D3D11_MAPPED_SUBRESOURCE mappedData2;//获取已经映射到缓冲区的内存
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData2));
	memcpy_s(mappedData2.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);

	//设置光栅化状态描述
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;// 线框填充方式
	rasterizerDesc.CullMode = D3D11_CULL_NONE;     //无背面裁剪，即三角形无论处在视野的正面还是背面都能看到
	rasterizerDesc.FrontCounterClockwise = FALSE;  //按照三角形顶点逆时针排布为正面 
	rasterizerDesc.DepthClipEnable = TRUE;         //深度测试范围外的像素剪裁掉
	HR(m_pd3d11Device->CreateRasterizerState(&rasterizerDesc, m_pRSWireframe.GetAddressOf()));

	// 给渲染管线各个阶段绑定好所需资源

	//渲染管线输入装配阶段设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout.Get());
	//将着色器绑定到渲染管线，给渲染管线某一着色阶段设置对应的着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	// VS常量缓冲区对应HLSL寄存于b0的常量缓冲区,PS常量缓冲区对应HLSL寄存于b1的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer[0].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());
	

	//设置调试对象名
	D3D11SetDebugObjectName(m_pVertexInputLayout.Get(), "VertexPosNormalColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "VertexShader");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "PixelShader");
	D3D11SetDebugObjectName(m_pConstantBuffer[0].Get(), "VSConstantBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffer[1].Get(), "PSConstantBuffer");

	return true;

}
