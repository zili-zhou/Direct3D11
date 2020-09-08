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
	//��ÿһ֡������ƵĲ����У�������Ҫ����һ����ȾĿ����ͼ�󶨵Ļ�����
	static float blue[4] = { 0.0f,0.0f,0.0f,1.0f };//�Ժ󱸻�����(R8G8B8A8)ʹ����ɫ�������
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);

	//ͬ���ڽ�����Ⱦ֮ǰ������ҲҪ����һ�����/ģ�建����
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present����--ǰ�󱸻���������������
	m_pd3dImmediateContext->Draw(3, 0);//��Ҫ���ƵĶ�����Ŀ,��ʼ��������
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
	HR(CreateShaderFromFile(L"HLSL\\Triangle_VS.cso", L"HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	//�������󶨶������벼��
	HR(m_pd3d11Device->CreateInputLayout(VertexPosColor::inputlayout, ARRAYSIZE(VertexPosColor::inputlayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexInputLayout.GetAddressOf()));

	//����������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\Triangle_PS.cso", L"HLSL\\Triangle_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
	
}

bool GameAPP::InitResource()
{
	
	// ���������ζ���
	// ע����������ĸ���˳��Ӧ����˳ʱ���Ų�
	VertexPosColor TriVertices[] =
	{
		//Triangle
		{ XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }

	};

	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;//vertex buffer description
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.ByteWidth = sizeof TriVertices;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	//�½����㻺������ʼ��������
	D3D11_SUBRESOURCE_DATA IniData;
	ZeroMemory(&IniData, sizeof(IniData));
	IniData.pSysMem = TriVertices;//// ���ڳ�ʼ��������
	HR(m_pd3d11Device->CreateBuffer(&vbd, &IniData, m_pVertexBuffer.GetAddressOf()));


	// ����Ⱦ���߸����׶ΰ󶨺�������Դ


	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(VertexPosColor); //��Խ�ֽ���
	UINT offset = 0;   //��ʼƫ����
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	//��Ⱦ��������װ��׶�����ͼԪ���ͣ��趨���벼��
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//ͼԪ����
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexInputLayout.Get());

	//����ɫ���󶨵���Ⱦ���ߣ�����Ⱦ����ĳһ��ɫ�׶����ö�Ӧ����ɫ��
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	//���õ��Զ�����
	D3D11SetDebugObjectName(m_pVertexInputLayout.Get(), "VertexPosColorInputLayout");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "VertexShader");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "PixelShader");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");


	return true;
	
}