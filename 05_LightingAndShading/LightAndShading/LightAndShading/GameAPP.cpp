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

	//��ʼ����꣬���̲���Ҫ
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	return true;
}

void GameAPP::OnResize()
{
	D3DApp::OnResize();
}

void GameAPP::UpdateScene(float dt)//dt����֮֡���ʱ����
{
	static float objPhi = 0.0f, objTheta = 0.0f;//���� X Y����ת��
	objPhi += 0.0004f; 
	objTheta += 0.0006f;

	XMMATRIX Matrix = XMMatrixRotationX(objPhi) * XMMatrixRotationY(objTheta);
	m_VSConstantBuffer.world = XMMatrixTranspose(Matrix);
	m_VSConstantBuffer.worldInvTranspose = XMMatrixTranspose(InverseTranspose(Matrix));



	//��ȡ����ʼ�����״̬
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();

	// ������갴ť״̬��������������갴ס������²Ž����ƶ�
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

	//�����л��ƹ�����
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

	// �����л�ģ������


	
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
		m_pd3dImmediateContext->RSSetState(m_IsWireFrameMode == true ? m_pRSWireframe.Get() : nullptr);//���ù�դ��״̬����Ϊnullptr����ʹ��Ĭ�Ϲ�դ��״̬
	}
	
	// DYNAMIC���³�����������������ת����
	D3D11_MAPPED_SUBRESOURCE mappedData;//��ȡ�Ѿ�ӳ�䵽���������ڴ�

	//��ȡָ�򻺳��������ݵ�ָ�벢�ܾ�GPU�Ըû������ķ���
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//ӳ��������ڴ����ǿ���ͨ��memcpy_s����������
	memcpy_s(mappedData.pData, sizeof(m_VSConstantBuffer), &m_VSConstantBuffer, sizeof(m_VSConstantBuffer));
	//��ָ����Դ��ָ����Ч����������GPU�Ը���Դ�ķ���Ȩ��
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);

	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(m_PSConstantBuffer), &m_PSConstantBuffer, sizeof(m_PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);	
}

bool GameAPP::ResetMesh(const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData)
{
	// �ͷž���Դ
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexPosNormalColor);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(m_pd3d11Device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(VertexPosNormalColor);	// ��Խ�ֽ���
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
	//��ÿһ֡������ƵĲ����У�������Ҫ����һ����ȾĿ����ͼ�󶨵Ļ�����
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::DarkGray));

	//ͬ���ڽ�����Ⱦ֮ǰ������ҲҪ����һ�����/ģ�建����
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present����--ǰ�󱸻���������������
	//������Ŀ����ʼ����λ�ã���ʼ����λ��
	m_pd3dImmediateContext->DrawIndexed(m_indexCount, 0, 0);
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
	HR(CreateShaderFromFile(L"HLSL\\VS_Light.cso", L"HLSL\\VS_Light.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	//�������󶨶������벼��
	HR(m_pd3d11Device->CreateInputLayout(VertexPosNormalColor::inputLayout, ARRAYSIZE(VertexPosNormalColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout.GetAddressOf()));
	//����������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\PS_Light.cso", L"HLSL\\PS_Light.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));


	return true;

}

bool GameAPP::InitResource()
{

	//��ʼ������ģ��
	auto meshData = Geometry::CreateBox<VertexPosNormalColor, DWORD>();

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
	
	//��ʼ��Ĭ�Ϲ���
	//�����
	m_DirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.direction = XMFLOAT3(-0.577f, -0.577f, 0.577f);
	
	// ���
	m_PointLight.position = XMFLOAT3(0.0f, 1.0f, -10.0f);
	m_PointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PointLight.att = XMFLOAT3(0.0f, 0.1f, 0.004f);
	m_PointLight.range = 25.0f;

	//�۹�

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

	//ʹ��Ĭ�Ϸ����
	m_PSConstantBuffer.directionalLight = m_DirLight;

	m_PSConstantBuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);

	// ����PS������������Դ

	D3D11_MAPPED_SUBRESOURCE mappedData2;//��ȡ�Ѿ�ӳ�䵽���������ڴ�
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData2));
	memcpy_s(mappedData2.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[1].Get(), 0);

	//���ù�դ��״̬����
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;// �߿���䷽ʽ
	rasterizerDesc.CullMode = D3D11_CULL_NONE;     //�ޱ���ü��������������۴�����Ұ�����滹�Ǳ��涼�ܿ���
	rasterizerDesc.FrontCounterClockwise = FALSE;  //���������ζ�����ʱ���Ų�Ϊ���� 
	rasterizerDesc.DepthClipEnable = TRUE;         //��Ȳ��Է�Χ������ؼ��õ�
	HR(m_pd3d11Device->CreateRasterizerState(&rasterizerDesc, m_pRSWireframe.GetAddressOf()));

	// ����Ⱦ���߸����׶ΰ󶨺�������Դ

	//��Ⱦ��������װ��׶�����ͼԪ���ͣ��趨���벼��
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout.Get());
	//����ɫ���󶨵���Ⱦ���ߣ�����Ⱦ����ĳһ��ɫ�׶����ö�Ӧ����ɫ��
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	// VS������������ӦHLSL�Ĵ���b0�ĳ���������,PS������������ӦHLSL�Ĵ���b1�ĳ���������
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer[0].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer[1].GetAddressOf());
	

	//���õ��Զ�����
	D3D11SetDebugObjectName(m_pVertexInputLayout.Get(), "VertexPosNormalColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "VertexShader");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "PixelShader");
	D3D11SetDebugObjectName(m_pConstantBuffer[0].Get(), "VSConstantBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffer[1].Get(), "PSConstantBuffer");

	return true;

}
