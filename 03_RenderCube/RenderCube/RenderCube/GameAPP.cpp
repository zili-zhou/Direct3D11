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
	static float phi = 0.0f, theta = 0.0f,psi=0.0f;//X��Y, Z�������ת��
	phi += 0.001f;
	theta += 0.0015f;
	psi += 0.00030f;
	m_CBuffer.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta) * XMMatrixRotationZ(psi));
	// DYNAMIC���³�������������������ת����
	D3D11_MAPPED_SUBRESOURCE mappedData;//��ȡ�Ѿ�ӳ�䵽���������ڴ�

	//��ȡָ�򻺳��������ݵ�ָ�벢�ܾ�GPU�Ըû������ķ���
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//ӳ��������ڴ����ǿ���ͨ��memcpy_s����������
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	//��ָ����Դ��ָ����Ч����������GPU�Ը���Դ�ķ���Ȩ��
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);

}

void GameAPP::DrawScene()
{

	assert(m_pSwapChain);
	assert(m_pd3dImmediateContext);
	//��ÿһ֡������ƵĲ����У�������Ҫ����һ����ȾĿ����ͼ�󶨵Ļ�����
	static float blue[4] = { 0.0f,0.0f,0.0f,1.0f };//�Ժ󱸻�����(R8G8B8A8)ʹ����ɫ�������
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&blue));

	//ͬ���ڽ�����Ⱦ֮ǰ������ҲҪ����һ�����/ģ�建����
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present����--ǰ�󱸻���������������
	//m_pd3dImmediateContext->DrawIndexed(54, 0, 0); //������Ŀ����ʼ����λ�ã���ʼ����λ��
	m_pd3dImmediateContext->DrawIndexed(36, 0, 0);
	m_pd3dImmediateContext->DrawIndexed(18, 36, 7);
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
	HR(CreateShaderFromFile(L"HLSL\\Cube_VS.cso", L"HLSL\\Cube_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	//�������󶨶������벼��
	HR(m_pd3d11Device->CreateInputLayout(VertexPosColor::inputlayout, ARRAYSIZE(VertexPosColor::inputlayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout.GetAddressOf()));
	//����������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\Cube_PS.cso", L"HLSL\\Cube_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));


	return true;

}

bool GameAPP::InitResource()
{
	// ******************
	// ���������嶥��
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

	//���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof CubeVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	//�½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = CubeVertices;
	HR(m_pd3d11Device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	//��������
	DWORD indices[] =
	{/*
		//����
		0,1,2,
		2,3,0,
		//����
		4,5,1,
		1,0,4,
		//����
		1,5,6,
		6,2,1,
		//����
		7,6,5,
		5,4,7,
		//����
		3,2,6,
		6,7,3,
		//����
		4,0,3,
		3,7,4,
		
		//Rectangular pyramid ����׶
		8, 12, 11,
		11, 12, 10,
		10, 12, 9,
		9, 12, 8,
		8, 11, 9, // ����2�ı������������������
		9, 11, 10 // ע������˳��
		*/
		// cube
		0, 1, 2,
		2, 3, 0, // ����
		4, 5, 1,
		1, 0, 4, // ����
		1, 5, 6,
		6, 2, 1, // ����
		3, 2, 6,
		6, 7, 3, // ����
		7, 6, 5,
		5, 4, 7, // ����
		3, 7, 4,
		4, 0, 3,  // ����
		//Rectangular pyramid ����׶
		1, 5, 4,
		4, 5, 3,
		3, 5, 2,
		2, 5, 1,
		1, 4, 2, // ����2�ı������������������
		2, 4, 3  // ע������˳��
	};

	//������������������
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof indices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	//�½�����������
	InitData.pSysMem = indices;
	HR(m_pd3d11Device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	//����װ���׶���������������
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	//���ó�������������
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// �½���������������ʹ�ó�ʼ����
	HR(m_pd3d11Device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.GetAddressOf()));

	//��ʼ��������������ֵ
	//����view��projective������ҪԤ�Ƚ���һ��ת���Ե���HLSL���������ת�ã�����world�����Ѿ��ǵ�λ����Ͳ���Ҫ��
	m_CBuffer.world = XMMatrixIdentity();//��λ����ת��Ϊ����
	//Builds a view matrix for a left-handed coordinate system using a camera position, a focal point, and an up direction.    
	m_CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH
	(
		XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f),
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
	m_CBuffer.projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI*0.6f, AspectRatio(), 1.0f, 1000.0f));//XM_PIDIV2��PI/2



	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(VertexPosColor);
	UINT offset = 0;
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//��Ⱦ��������װ��׶�����ͼԪ���ͣ��趨���벼��
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout.Get());
	//����ɫ���󶨵���Ⱦ���ߣ�����Ⱦ����ĳһ��ɫ�׶����ö�Ӧ����ɫ��
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	//������ɫ�׶����ó�����������ʹ��
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
