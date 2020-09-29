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

	//��ʼ����꣬���̲���Ҫ
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	return true;
}

void GameAPP::OnResize()
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

	if (hr == S_OK)
	{
		// �����̶���ɫˢ���ı���ʽ
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_pSolidColorBrush.GetAddressOf()));
		HR(m_pdWriteFactory->CreateTextFormat(L"΢���ź�", nullptr,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"zh-cn", m_pTextFormat.GetAddressOf()));
	}
	else
		assert(m_pd2dRenderTarget);
}

void GameAPP::UpdateScene(float dt)//dt����֮֡���ʱ����
{
	Keyboard::State keyboardState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyboardState);

	//�����л�ģʽ
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D1) && m_CurrMode != ShowMode::WoodCreate)
	{
		//����ľ��
		//��������ֱ�Ӱ󶨵���ɫ���У���ҪΪ��������Ӧ����ɫ����Դ��ͼ���ܹ�����ɫ��ʹ��
		m_CurrMode = ShowMode::WoodCreate;
		auto meshData = Geometry::CreateBox<VertexPosNormalTexture,DWORD>();
		ResetMesh(meshData);
		m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());
		m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCreate.GetAddressOf());//��ʼ����������ӦHLSL��register(t*);��ɫ����Դ��ͼ��Ŀ;��ɫ����Դ��ͼ����
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::D2) && m_CurrMode != ShowMode::FireCreate)
	{
		//����ľ�����
		//��������ֱ�Ӱ󶨵���ɫ���У���ҪΪ��������Ӧ����ɫ����Դ��ͼ���ܹ�����ɫ��ʹ��
		m_CurrMode = ShowMode::FireCreate;
		auto meshData = Geometry::CreateBox<VertexPosNormalTexture, DWORD>();
		ResetMesh(meshData);
		m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());
		m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pFireCreate.GetAddressOf());//��ʼ����������ӦHLSL��register(t*);��ɫ����Դ��ͼ��Ŀ;��ɫ����Դ��ͼ����
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
		static float objPhi = 0.0f, objTheta = 0.0f;//���� X Y����ת��
		objPhi += 0.0002f;
		objTheta += 0.0003f;
		XMMATRIX Matrix = XMMatrixRotationX(objPhi) * XMMatrixRotationY(objTheta);
		m_VSConstantBuffer.world = XMMatrixTranspose(Matrix);
		m_VSConstantBuffer.worldInvTranspose = XMMatrixTranspose(InverseTranspose(Matrix));

		// DYNAMIC���³�����������������ת����
		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(m_VSConstantBuffer), &m_VSConstantBuffer, sizeof(m_VSConstantBuffer));
		m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);
	}
	else if (m_CurrMode == ShowMode::FireAnimation)
	{
		// ����������1��60֡
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
	// �ͷž���Դ
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexType);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(m_pd3d11Device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(VertexType);	// ��Խ�ֽ���
	UINT offset = 0;							// ��ʼƫ����

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);



	// ������������������
	m_indexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_indexCount * sizeof(DWORD);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// �½�����������
	InitData.pSysMem = meshData.indexVec.data();
	HR(m_pd3d11Device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// ����װ��׶ε���������������
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);



	// ���õ��Զ�����
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

	return true;
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
	m_pd3dImmediateContext->DrawIndexed(m_indexCount, 0, 0);

	//����Direct2D����
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring textStr = L"�л���ʾ: 1-ľ��(3D) 2-����ľ��(3D) 3-����(2D)\n";
		m_pd2dRenderTarget->DrawTextW(textStr.c_str(), textStr.size(), m_pTextFormat.Get(), D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pSolidColorBrush.Get());
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

	//��ʼ������ģ��
	auto meshData = Geometry::CreateBox<VertexPosNormalTexture, DWORD>();

	ResetMesh(meshData);

	//�ֱ���������VS,PS�ĳ�������������
	//���㣬��������������ResetMesh��
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[0].GetAddressOf()));

	cbd.ByteWidth = sizeof(PSConstantBuffer);
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer[1].GetAddressOf()));

	//��ʼ������VS�ĳ�����������ֵ

	//����view��projective������ҪԤ�Ƚ���һ��ת���Ե���HLSL���������ת�ã�����world�����Ѿ��ǵ�λ����Ͳ���Ҫ��
	m_VSConstantBuffer.world = XMMatrixIdentity();//��λ����ת��Ϊ����
	//Builds a view matrix for a left-handed coordinate system using a camera position, a focal point, and an up direction.    
	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH
	(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));
	//�����ӳ���������͸��ͶӰ����
	//XMMATRIX XM_CALLCONV XMMatrixPerspectiveFovLH(
		//float FovAngleY,���϶��µ��ӳ��ǣ��Ի���Ϊ��λ����
		//float AspectRatio,�ӿռ�Ŀ�߱�X��Y
		//float NearZ,��ƽ�棨����
		//float FarZ Զƽ�棨����
	//);
	m_VSConstantBuffer.projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI*0.6f, AspectRatio(), 1.0f, 1000.0f));//XM_PIDIV2��PI/2
	m_VSConstantBuffer.worldInvTranspose = XMMatrixIdentity();

	// ����VS������������Դ
	D3D11_MAPPED_SUBRESOURCE mappedData1;//��ȡ�Ѿ�ӳ�䵽���������ڴ�
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData1));
	memcpy_s(mappedData1.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);


	//��ʼ������PS�ĳ�����������ֵ
	
	// ��ʼ������PS�ĳ�����������ֵ
	// ����ֻʹ��һյ�������ʾ
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

	// ����PS������������Դ

	D3D11_MAPPED_SUBRESOURCE mappedData2;//��ȡ�Ѿ�ӳ�䵽���������ڴ�
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData2));
	memcpy_s(mappedData2.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);

	// ��ʼ������Ͳ�����״̬

	//��ʼ��ľ������
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\WoodCrate.dds", nullptr, m_pWoodCreate.GetAddressOf()));

	//��ʼ������ľ��
	HR(CreateDDSTextureFromFile(m_pd3d11Device.Get(), L"..\\Texture\\flare.dds", nullptr, m_pFireCreate.GetAddressOf()));

	//��ʼ����������
	WCHAR strFile[40];
	m_pFireAnim.resize(120);
	for (int i = 1; i <= 120; i++)
	{
		wsprintf(strFile, L"..\\Texture\\FireAnim\\Fire%03d.bmp", i);
		HR(CreateWICTextureFromFile(m_pd3d11Device.Get(), strFile, nullptr, m_pFireAnim[static_cast<size_t>(i) - 1].GetAddressOf()));
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

	// ����Ⱦ���߸����׶ΰ󶨺�������Դ

	//��Ⱦ��������װ��׶�����ͼԪ���ͣ��趨���벼��
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout3D.Get());
	//Ĭ�ϰ�3D��ɫ��
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
	// VS������������ӦHLSL�Ĵ���b0�ĳ���������,PS������������ӦHLSL�Ĵ���b1�ĳ���������
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer[0].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());

	// ������ɫ�׶����úò�����
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCreate.GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);

	//���õ��Զ�����
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
