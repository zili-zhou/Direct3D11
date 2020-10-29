
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

	//��ʼ����꣬���̲���Ҫ,ʹ�������ģʽ����ʱ������ǲ��ɼ��ġ���ʱ����Խ����ģʽ��Ϊ���ģʽ��
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameAPP::OnResize()//�����������������׶����ӿڵķ�����������Ҫ���³����������е�ͶӰ����
{
	assert(m_pd2dFactory);
	assert(m_pdWriteFactory);

	// �ͷ�D2D�������Դ
	m_pd2dRenderTarget.Reset();
	m_pSolidColorBrush.Reset();

	//D2D�������Դ��Ҫ��D3D�����Դ�ͷ�ǰ�����ͷŵ���Ȼ����D3D����󱸻����������´���D2D��ȾĿ��
	D3DApp::OnResize();

	// ΪD2D����DXGI������ȾĿ��

	//ͨ��IDXGISwapChain::GetBuffer��������ȡ�󱸻�������IDXGISurface�ӿ�
	ComPtr<IDXGISurface> surface;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugStringW(L"\n���棺Direct2D��Direct3D�������Թ������ޣ��㽫�޷������ı���Ϣ�����ṩ������ѡ������\n"
			L"1. ����Win7ϵͳ����Ҫ������Win7 SP1������װKB2670838������֧��Direct2D��ʾ��\n"
			L"2. �������Direct3D 10.1��Direct2D�Ľ�����������ģ�"
			L"https://docs.microsoft.com/zh-cn/windows/desktop/Direct2D/direct2d-and-direct3d-interoperation-overview""\n"
			L"3. ʹ�ñ������⣬����FreeType��\n\n");
	}
	else if (hr == S_OK)
	{
		// �����̶���ɫˢ���ı���ʽ
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_pSolidColorBrush.GetAddressOf()));
		HR(m_pdWriteFactory->CreateTextFormat(L"΢���ź�", nullptr,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn", m_pTextFormat.GetAddressOf()));
	}
	else
		assert(m_pd2dRenderTarget);

	//����������ʾ
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


//��ȡ���λ�ƣ������ݵ�ǰ�������ģʽ�ͼ��������״̬�����ж�Ӧ����
void GameAPP::UpdateScene(float dt)//dt����֮֡���ʱ����
{
	//��������¼���ȡ���ƫ����
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastmouseState = m_MouseTracker.GetLastState();

	Keyboard::State keyboardState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyboardState);


	//��ȡ�������
	auto cameraFirst = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);//��һ���������ܶ���ָ�루�����ã�cast���̳���ָ��
	auto cameraThird = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	Transform& woodCrateTransform = m_WoodCrate.GetTransform();

	//��һ�˳�/���������
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
		// �������λ��������[-8.9, 8.9]x[0.0, 8.9]x[-8.9, 8.9]��������
		// ��������
		XMFLOAT3 adjustPos;
		XMStoreFloat3(&adjustPos, XMVectorClamp(cameraFirst->GetPositionXM(), XMVectorSet(-8.9f, 0.0f, -8.9f, 0.0f), XMVectorReplicate(8.9f)));
		cameraFirst->SetPosition(adjustPos);

		// ���ڵ�һ�˳�ģʽ�ƶ��������ͬʱ�ƶ�����
		if (m_CameraMode == CameraMode::FirstPersonCamera)
		{
			woodCrateTransform.SetPosition(adjustPos);
		}
		// �����û���봰��ǰ��ΪABSOLUTEģʽ
		if (mouseState.positionMode == Mouse::Mode::MODE_RELATIVE)
		{
			cameraFirst->RotateX(mouseState.y * dt * 2.5f);
			cameraFirst->RotateY(mouseState.x * dt * 2.5f);
		}
	}
	else if (m_CameraMode == CameraMode::ThirdPersonCamera)//�����˳�
	{
		cameraThird->SetTargetPosition(woodCrateTransform.GetPosition());

		//��������ת
		cameraThird->SetRotateX(mouseState.y * dt * 2.5f);
		cameraThird->SetRotateY(mouseState.x * dt * 2.5f);
		cameraThird->Approach(-(float)mouseState.scrollWheelValue / 120.0f);
	}

	// ���¹۲����
	XMStoreFloat4(&m_CBFrame.eyePos, m_pCamera->GetPositionXM());
	m_CBFrame.view = XMMatrixTranspose(m_pCamera->GetViewXM());

	//���ù���ֵ
	m_pMouse->ResetScrollWheelValue();

	//�����ģʽ���л�
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
		//�������Ϸ���ʼ
		XMFLOAT3 pos = woodCrateTransform.GetPosition();
		pos.y += 3.0f;
		cameraFirst->LookTo(pos, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_CameraMode = CameraMode::Free;
	}
	// �˳���������Ӧ�򴰿ڷ���������Ϣ
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
	
	//����Direct3D����

	//��ÿһ֡������ƵĲ����У�������Ҫ����һ����ȾĿ����ͼ�󶨵Ļ�����
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));

	//ͬ���ڽ�����Ⱦ֮ǰ������ҲҪ����һ�����/ģ�建����
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present����--ǰ�󱸻���������������
	//���Ƽ������壬������Ŀ����ʼ����λ�ã���ʼ����λ��
	m_WoodCrate.Draw(m_pd3dImmediateContext.Get());
	m_Floor.Draw(m_pd3dImmediateContext.Get());
	for (auto& wall : m_Walls)
		wall.Draw(m_pd3dImmediateContext.Get());

	//����Direct2D����
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring text = L"�л������ģʽ: 1-��һ�˳� 2-�����˳� 3-�����ӽ�\n"
			L"W/S/A/D ǰ��/����/��ƽ��/��ƽ�� (�����˳���Ч)  Esc�˳�\n"
			L"����ƶ�������Ұ ���ֿ��Ƶ����˳ƹ۲����\n"
			L"��ǰģʽ: ";
		if (m_CameraMode == CameraMode::FirstPersonCamera)
			text += L"��һ�˳�(���������ƶ�)";
		else if (m_CameraMode == CameraMode::ThirdPersonCamera)
			text += L"�����˳�";
		else
			text += L"�����ӽ�";
		m_pd2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pSolidColorBrush.Get());
		HR(m_pd2dRenderTarget->EndDraw());
	}

	HR(m_pSwapChain->Present(0, 0));

}

bool GameAPP::InitEffect()
{

	ComPtr<ID3DBlob> blob;

	//����������ɫ��
	// ------------------------------
	// CreateShaderFromFile����
	// ------------------------------
	// [In]csoFileNameInOut ����õ���ɫ���������ļ�(.cso)������ָ��������Ѱ�Ҹ��ļ�����ȡ
	// [In]hlslFileName     ��ɫ�����룬��δ�ҵ���ɫ���������ļ��������ɫ������
	// [In]entryPoint       ��ڵ�(ָ����ʼ�ĺ���)
	// [In]shaderModel      ��ɫ��ģ�ͣ���ʽΪ"*s_5_0"��*����Ϊc,d,g,h,p,v֮һ
	// [Out]ppBlobOut       �����ɫ����������Ϣ

	//����2D������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\VS_2D.cso", L"HLSL\\VS_2D.hlsl", "VS_2D", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader2D.GetAddressOf()));
	//�������󶨶������벼��
	HR(m_pd3d11Device->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout2D.GetAddressOf()));

	//����3D������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\VS_3D.cso", L"HLSL\\VS_3D.hlsl", "VS_3D", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader3D.GetAddressOf()));
	//�������󶨶������벼��
	HR(m_pd3d11Device->CreateInputLayout(VertexPosNormalTexture::inputLayout, ARRAYSIZE(VertexPosNormalTexture::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout3D.GetAddressOf()));

	//����2D������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\PS_2D.cso", L"HLSL\\PS_2D.hlsl", "PS_2D", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader2D.GetAddressOf()));

	//����3D������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\PS_3D.cso", L"HLSL\\PS_3D.hlsl", "PS_3D", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader3D.GetAddressOf()));

	return true;

}

bool GameAPP::InitResource()
{

	//�ֱ��������ڶ��ֵĳ�������������
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

	//��ʼ����Ϸ����
	ComPtr<ID3D11ShaderResourceView> texture;

	//��ʼ��ľ��
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	m_WoodCrate.SetBuffer(m_pd3d11Device.Get(), Geometry::CreateBox());
	m_WoodCrate.SetTexture(texture.Get());

	//��ʼ���ذ�
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\floor.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Floor.SetBuffer(m_pd3d11Device.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	m_Floor.SetTexture(texture.Get());
	m_Floor.GetTransform().SetPosition(XMFLOAT3(0.0f, -1.0f, 0.0f));

	//��ʼ��ǽ��
	m_Walls.resize(4);
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// �������ǽ���ĸ��������
	for (int i = 0; i < 4; i++)
	{
		m_Walls[i].SetBuffer(m_pd3d11Device.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
		m_Walls[i].SetTexture(texture.Get());
		Transform& transform = m_Walls[i].GetTransform();
		transform.SetRotationEuler(-XM_PIDIV2, XM_PIDIV2 * i, 0.0f);
		transform.SetPosition(i % 2 ? -10.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -10.0f * (i - 1) : 0.0f);
	}

	//��ʼ��������״̬
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

	
	//��ʼ��������������ֵ
	
	//��ʼ��ÿ֡�仯��ֵ
	m_CameraMode = CameraMode::FirstPersonCamera;
	auto camera=std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera());
	m_pCamera = camera;//�������ֵ���������
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->LookAt(XMFLOAT3(), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	//��ʼ�����ڴ�С�仯ʱ�޸ĵ�ֵ�����³�����������Դ
	m_pCamera->SetFrustum(XM_PI / 3.0f, AspectRatio(), 0.5f, 1000.0f);
	m_CBOnResize.projection = XMMatrixTranspose(m_pCamera->GetProjXM());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesOnresize), &m_CBOnResize, sizeof(CBChangesOnresize));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[2].Get(), 0);


	//��ʼ������仯��ֵ�����³�����������Դ
	//�����
	m_CBRarely.directionalLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.directionalLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_CBRarely.directionalLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.directionalLight[0].direction = XMFLOAT3(0.0f, -1.0f, 0.0f)	;
	
	//���Դ
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


	// ����Ⱦ���߸����׶ΰ󶨺�������Դ
	//��Ⱦ��������װ��׶�����ͼԪ���ͣ��趨���벼��
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());

	//��Ĭ��3D��ɫ��
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);

	//Ԥ�Ȱ󶨸�������Ļ�����������ÿ֡���µĻ�������Ҫ�󶨵�VS PS������������
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer[0].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(2, 1, m_pConstantBuffer[2].GetAddressOf());

	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(3, 1, m_pConstantBuffer[3].GetAddressOf());
	// ������ɫ�׶����úò�����
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);

	

	//���õ��Զ�����
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
	// �ͷž���Դ
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// ���ö��㻺��������
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));



	// ������������������
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// �½�����������
	InitData.pSysMem = meshData.indexVec.data();
	HR(device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));

	// ���õ��Զ�����
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

}


void GameAPP::GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}

void GameAPP::GameObject::Draw(ID3D11DeviceContext* deviceContext)//�ڻ���һ������ʱ����Ҫ���µ������г���������������Ҫ�л����������������㻺����������������
{
	// ����װ��׶εĶ���/��������������
	UINT stride = m_VertexStride;	// ��Խ�ֽ���
	UINT offset = 0;				// ��ʼƫ����
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// ��ȡ֮ǰ�Ѿ��󶨵���Ⱦ�����ϵĳ����������������޸�
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());// ��ȡĳһ��ɫ�׶εĳ���������
	CBChangesEveryDrawing CBDrawing;

	// �ڲ�����ת��
	XMMATRIX world = m_Transform.GetLocalToWorldMatrixXM();
	CBDrawing.world = XMMatrixTranspose(world);
	CBDrawing.worldInvTranspose = XMMatrixTranspose(InverseTranspose(world));

	//���³���������
	D3D11_MAPPED_SUBRESOURCE mappedData;//��ȡ�Ѿ�ӳ�䵽���������ڴ�
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryDrawing), &CBDrawing, sizeof(CBChangesEveryDrawing));
	deviceContext->Unmap(cBuffer.Get(), 0);

	//��������
	deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());

	//���Ƽ������壬������Ŀ����ʼ����λ�ã���ʼ����λ��
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
