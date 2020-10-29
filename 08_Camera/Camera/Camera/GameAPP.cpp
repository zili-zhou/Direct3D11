
#include"GameAPP.h"
#include"D3DUtil.h"
#include"DXTrace.h"
#include<assert.h>

using namespace DirectX;


GameAPP::GameAPP(HINSTANCE hInstance) :D3DApp(hInstance),
	m_CameraMode(CameraMode::FirstPersonCamera),m_CBFrame(), m_CBOnResize(),m_CBRarely()
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

	//初始化鼠标，键盘不需要,使用摄像机模式游玩时，鼠标是不可见的。这时候可以将鼠标模式设为相对模式。
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameAPP::OnResize()//摄像机保留有设置视锥体和视口的方法，并且需要更新常量缓冲区中的投影矩阵
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

	if (hr == E_NOINTERFACE)
	{
		OutputDebugStringW(L"\n警告：Direct2D与Direct3D互操作性功能受限，你将无法看到文本信息。现提供下述可选方法：\n"
			L"1. 对于Win7系统，需要更新至Win7 SP1，并安装KB2670838补丁以支持Direct2D显示。\n"
			L"2. 自行完成Direct3D 10.1与Direct2D的交互。详情参阅："
			L"https://docs.microsoft.com/zh-cn/windows/desktop/Direct2D/direct2d-and-direct3d-interoperation-overview""\n"
			L"3. 使用别的字体库，比如FreeType。\n\n");
	}
	else if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_pSolidColorBrush.GetAddressOf()));
		HR(m_pdWriteFactory->CreateTextFormat(L"微软雅黑", nullptr,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn", m_pTextFormat.GetAddressOf()));
	}
	else
		assert(m_pd2dRenderTarget);

	//摄像机变更显示
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetFrustum(XM_PI / 3.0f, AspectRatio(), 0.5f, 1000.0f);
		m_pCamera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
		m_CBOnResize.projection = XMMatrixTranspose(m_pCamera->GetProjXM());

		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[2].Get(), 0, D3D11_MAP_WRITE_DISCARD,0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(CBChangesOnresize), &m_CBOnResize, sizeof(CBChangesOnresize));
		m_pd3dImmediateContext->Unmap(m_pConstantBuffer[2].Get(), 0);
	}
}


//获取相对位移，并根据当前摄像机的模式和键鼠操作的状态来进行对应操作
void GameAPP::UpdateScene(float dt)//dt是两帧之间的时间间隔
{
	//更新鼠标事件获取相对偏移量
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastmouseState = m_MouseTracker.GetLastState();

	Keyboard::State keyboardState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyboardState);


	//获取相机子类
	auto cameraFirst = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);//将一个基类智能对象指针（或引用）cast到继承类指针
	auto cameraThird = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	Transform& woodCrateTransform = m_WoodCrate.GetTransform();

	//第一人称/自由摄像机
	if (m_CameraMode == CameraMode::FirstPersonCamera || m_CameraMode == CameraMode::Free)
	{
		if (keyboardState.IsKeyDown(Keyboard::Keys::W))
		{
			if (m_CameraMode == CameraMode::FirstPersonCamera)
			{
				cameraFirst->WatchForward(3.0f * dt);
			}
			else
			{
				cameraFirst->MoveForward(3.0f * dt);
			}
		}
		if (keyboardState.IsKeyDown(Keyboard::Keys::S))
		{
			if (m_CameraMode == CameraMode::FirstPersonCamera)
			{
				cameraFirst->WatchForward(-3.0f * dt);
			}
			else
			{
				cameraFirst->MoveForward(-3.0f * dt);
			}
		}
		if (keyboardState.IsKeyDown(Keyboard::Keys::A))
		{
			cameraFirst->MoveLeftRight(-3.0f * dt);
		}
		if (keyboardState.IsKeyDown(Keyboard::Keys::D))
		{
			cameraFirst->MoveLeftRight(3.0f * dt);
		}
		// 将摄像机位置限制在[-8.9, 8.9]x[0.0, 8.9]x[-8.9, 8.9]的区域内
		// 不允许穿地
		XMFLOAT3 adjustPos;
		XMStoreFloat3(&adjustPos, XMVectorClamp(cameraFirst->GetPositionXM(), XMVectorSet(-8.9f, 0.0f, -8.9f, 0.0f), XMVectorReplicate(8.9f)));
		cameraFirst->SetPosition(adjustPos);

		// 仅在第一人称模式移动摄像机的同时移动箱子
		if (m_CameraMode == CameraMode::FirstPersonCamera)
		{
			woodCrateTransform.SetPosition(adjustPos);
		}
		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::Mode::MODE_RELATIVE)
		{
			cameraFirst->RotateX(mouseState.y * dt * 2.5f);
			cameraFirst->RotateY(mouseState.x * dt * 2.5f);
		}
	}
	else if (m_CameraMode == CameraMode::ThirdPersonCamera)//第三人称
	{
		cameraThird->SetTargetPosition(woodCrateTransform.GetPosition());

		//绕物体旋转
		cameraThird->SetRotateX(mouseState.y * dt * 2.5f);
		cameraThird->SetRotateY(mouseState.x * dt * 2.5f);
		cameraThird->Approach(-(float)mouseState.scrollWheelValue / 120.0f);
	}

	// 更新观察矩阵
	XMStoreFloat4(&m_CBFrame.eyePos, m_pCamera->GetPositionXM());
	m_CBFrame.view = XMMatrixTranspose(m_pCamera->GetViewXM());

	//重置滚轮值
	m_pMouse->ResetScrollWheelValue();

	//摄像机模式的切换
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D1) && m_CameraMode != CameraMode::FirstPersonCamera)
	{
		if (!cameraFirst)
		{
			cameraFirst.reset(new FirstPersonCamera);
			cameraFirst->SetFrustum(XM_PI / 3.0f, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cameraFirst;
		}
		cameraFirst->LookTo(woodCrateTransform.GetPosition(), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_CameraMode = CameraMode::FirstPersonCamera;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D2) && m_CameraMode != CameraMode::ThirdPersonCamera)
	{
		if (!cameraThird)
		{
			cameraThird.reset(new ThirdPersonCamera);
			cameraThird->SetFrustum(XM_PI / 3.0f, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cameraThird;
		}
		XMFLOAT3 target = woodCrateTransform.GetPosition();
		cameraThird->SetTargetPosition(target);
		cameraThird->SetDistance(8.0f);
		cameraThird->SetDistanceMinMax(3.0f, 20.0f);
		m_CameraMode = CameraMode::ThirdPersonCamera;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D3) && m_CameraMode != CameraMode::Free)
	{
		if (!cameraFirst)
		{
			cameraFirst.reset(new FirstPersonCamera);
			cameraFirst->SetFrustum(XM_PI / 3.0f, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cameraFirst;
		}
		//从箱子上方开始
		XMFLOAT3 pos = woodCrateTransform.GetPosition();
		pos.y += 3.0f;
		cameraFirst->LookTo(pos, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_CameraMode = CameraMode::Free;
	}
	// 退出程序，这里应向窗口发送销毁信息
	if (keyboardState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWND(), WM_DESTROY, 0, 0);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryFrame), &m_CBFrame, sizeof(CBChangesEveryFrame));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);
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
	m_WoodCrate.Draw(m_pd3dImmediateContext.Get());
	m_Floor.Draw(m_pd3dImmediateContext.Get());
	for (auto& wall : m_Walls)
		wall.Draw(m_pd3dImmediateContext.Get());

	//绘制Direct2D部分
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring text = L"切换摄像机模式: 1-第一人称 2-第三人称 3-自由视角\n"
			L"W/S/A/D 前进/后退/左平移/右平移 (第三人称无效)  Esc退出\n"
			L"鼠标移动控制视野 滚轮控制第三人称观察距离\n"
			L"当前模式: ";
		if (m_CameraMode == CameraMode::FirstPersonCamera)
			text += L"第一人称(控制箱子移动)";
		else if (m_CameraMode == CameraMode::ThirdPersonCamera)
			text += L"第三人称";
		else
			text += L"自由视角";
		m_pd2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pSolidColorBrush.Get());
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

	//分别设置用于多种的常数缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	cbd.ByteWidth = sizeof(CBChangesEveryDrawing);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[0].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesEveryFrame);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[1].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesOnresize);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[2].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesRarely);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[3].GetAddressOf()));

	//初始化游戏对象
	ComPtr<ID3D11ShaderResourceView> texture;

	//初始化木箱
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	m_WoodCrate.SetBuffer(m_pd3d11Device.Get(), Geometry::CreateBox());
	m_WoodCrate.SetTexture(texture.Get());

	//初始化地板
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\floor.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Floor.SetBuffer(m_pd3d11Device.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	m_Floor.SetTexture(texture.Get());
	m_Floor.GetTransform().SetPosition(XMFLOAT3(0.0f, -1.0f, 0.0f));

	//初始化墙体
	m_Walls.resize(4);
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// 这里控制墙体四个面的生成
	for (int i = 0; i < 4; i++)
	{
		m_Walls[i].SetBuffer(m_pd3d11Device.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
		m_Walls[i].SetTexture(texture.Get());
		Transform& transform = m_Walls[i].GetTransform();
		transform.SetRotationEuler(-XM_PIDIV2, XM_PIDIV2 * i, 0.0f);
		transform.SetPosition(i % 2 ? -10.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -10.0f * (i - 1) : 0.0f);
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

	
	//初始化常量缓冲区的值
	
	//初始化每帧变化的值
	m_CameraMode = CameraMode::FirstPersonCamera;
	auto camera=std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera());
	m_pCamera = camera;//子类对象赋值给父类对象
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->LookAt(XMFLOAT3(), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	//初始化窗口大小变化时修改的值并更新常量缓冲区资源
	m_pCamera->SetFrustum(XM_PI / 3.0f, AspectRatio(), 0.5f, 1000.0f);
	m_CBOnResize.projection = XMMatrixTranspose(m_pCamera->GetProjXM());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesOnresize), &m_CBOnResize, sizeof(CBChangesOnresize));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[2].Get(), 0);


	//初始化不会变化的值并更新常量缓冲区资源
	//方向光
	m_CBRarely.directionalLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.directionalLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_CBRarely.directionalLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.directionalLight[0].direction = XMFLOAT3(0.0f, -1.0f, 0.0f)	;
	
	//点光源
	m_CBRarely.pointLight[0].position = XMFLOAT3(0.0f, 10.0f, -10.0f);
	m_CBRarely.pointLight[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_CBRarely.pointLight[0].diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_CBRarely.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_CBRarely.pointLight[0].range = 25.0f;
	
	m_CBRarely.NumDirectionalLight = 1;
	m_CBRarely.NumPointLight = 1;
	m_CBRarely.NumSpotLight = 0;

	m_CBRarely.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.material.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_CBRarely.material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//D3D11_MAPPED_SUBRESOURCE mappedData2;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[3].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesRarely), &m_CBRarely, sizeof(CBChangesRarely));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[3].Get(), 0);


	// 给渲染管线各个阶段绑定好所需资源
	//渲染管线输入装配阶段设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());

	//绑定默认3D着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);

	//预先绑定各自所需的缓冲区，其中每帧更新的缓冲区需要绑定到VS PS两个缓冲区上
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer[0].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(2, 1, m_pConstantBuffer[2].GetAddressOf());

	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(3, 1, m_pConstantBuffer[3].GetAddressOf());
	// 像素着色阶段设置好采样器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);

	

	//设置调试对象名
	D3D11SetDebugObjectName(m_pVertexInputLayout3D.Get(), "VertexPosNormalColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexInputLayout2D.Get(), "VertexPosTextureInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader2D.Get(), "VertexShader2D");
	D3D11SetDebugObjectName(m_pVertexShader3D.Get(), "VertexShader3D");
	D3D11SetDebugObjectName(m_pPixelShader2D.Get(), "PixelShader2D");
	D3D11SetDebugObjectName(m_pPixelShader3D.Get(), "PixelShader3D");
	D3D11SetDebugObjectName(m_pConstantBuffer[0].Get(), "CBDrawing");
	D3D11SetDebugObjectName(m_pConstantBuffer[1].Get(), "CBFrame");
	D3D11SetDebugObjectName(m_pConstantBuffer[2].Get(), "CBResize");
	D3D11SetDebugObjectName(m_pConstantBuffer[3].Get(), "CBRarely");
	D3D11SetDebugObjectName(m_pSamplerState.Get(), "SSLinearWrap");

	m_Floor.SetDebugObjectName("Floor");
	m_WoodCrate.SetDebugObjectName("WoodCrate");
	m_Walls[0].SetDebugObjectName("Walls[0]");
	m_Walls[1].SetDebugObjectName("Walls[1]");
	m_Walls[2].SetDebugObjectName("Walls[2]");
	m_Walls[3].SetDebugObjectName("Walls[3]");

	return true;
}



GameAPP::GameObject::GameObject():m_VertexStride(),m_IndexCount(){}

Transform& GameAPP::GameObject::GetTransform()
{
	return m_Transform;
}

const Transform& GameAPP::GameObject::GetTransform()const
{
	return m_Transform;
}

template<class VertexType, class IndexType>
void GameAPP::GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));



	// 设置索引缓冲区描述
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = meshData.indexVec.data();
	HR(device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));

	// 设置调试对象名
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

}


void GameAPP::GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}

void GameAPP::GameObject::Draw(ID3D11DeviceContext* deviceContext)//在绘制一个对象时，需要更新的数据有常量缓冲区，而需要切换的数据有纹理、顶点缓冲区和索引缓冲区
{
	// 输入装配阶段的顶点/索引缓冲区设置
	UINT stride = m_VertexStride;	// 跨越字节数
	UINT offset = 0;				// 起始偏移量
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 获取之前已经绑定到渲染管线上的常量缓冲区并进行修改
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());// 获取某一着色阶段的常量缓冲区
	CBChangesEveryDrawing CBDrawing;

	// 内部进行转置
	XMMATRIX world = m_Transform.GetLocalToWorldMatrixXM();
	CBDrawing.world = XMMatrixTranspose(world);
	CBDrawing.worldInvTranspose = XMMatrixTranspose(InverseTranspose(world));

	//更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedData;//获取已经映射到缓冲区的内存
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryDrawing), &CBDrawing, sizeof(CBChangesEveryDrawing));
	deviceContext->Unmap(cBuffer.Get(), 0);

	//设置纹理
	deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());

	//绘制几何形体，索引数目，起始索引位置，起始顶点位置
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void GameAPP::GameObject::SetDebugObjectName(const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), name + ".VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), name + ".IndexBuffer");
#else
	UNREFERENCED_PARAMETER(name);
#endif
}
