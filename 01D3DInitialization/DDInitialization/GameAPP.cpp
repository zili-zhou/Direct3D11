#include"GameAPP.h"
//#include"D3DUtil.h"
#include<assert.h>
GameAPP::GameAPP(HINSTANCE hInstance):D3DApp(hInstance)
{

}

GameAPP::~GameAPP()
{}

bool GameAPP::Init()
{
	if (!D3DApp::Init())
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
	static float blue[4] = { 0.5f,0.4f,0.7f,0.3f };//�Ժ󱸻�����(R8G8B8A8)ʹ����ɫ�������
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);

	//ͬ���ڽ�����Ⱦ֮ǰ������ҲҪ����һ�����/ģ�建����
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present����--ǰ�󱸻���������������
	HR(m_pSwapChain->Present(0, 0));


}