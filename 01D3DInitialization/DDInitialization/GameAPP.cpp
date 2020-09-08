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
	//在每一帧画面绘制的操作中，我们需要清理一遍渲染目标视图绑定的缓冲区
	static float blue[4] = { 0.5f,0.4f,0.7f,0.3f };//对后备缓冲区(R8G8B8A8)使用蓝色进行清空
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);

	//同样在进行渲染之前，我们也要清理一遍深度/模板缓冲区
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//IDXGISwapChain::Present方法--前后备缓冲区交换并呈现
	HR(m_pSwapChain->Present(0, 0));


}