#include"D3DApp.h"
#include"D3DUtil.h"
#include<assert.h>
#include"DXTrace.h"
#include<sstream>

namespace
{
	D3DApp* g_pd3dApp=nullptr;//ȫ�ֱ���,�ڴ��ڴ�����ʱ����Ҫ��һ���ص�����
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return g_pd3dApp->MsgProc(hwnd, msg, wparam, lparam);
}

D3DApp::D3DApp(HINSTANCE hInstance) :
	m_hAppInstance(hInstance),
	m_hMainWnd(nullptr),
	m_AppPause(false),
	m_AppMaximize(false),
	m_AppMinimize(false),
	m_AppResize(false),
	m_Enable4xMSAA(true),
	m_4xMSAAQuality(0),
	m_MainWNDCaption(L"First and ThirdPerson Camera"),
	m_ClientWidth(800),
	m_ClientHeight(600),
	m_pd3d11Device(nullptr),
	m_pd3dImmediateContext(nullptr),
	m_pSwapChain(nullptr),
	m_pDepthStencilBuffer(nullptr),
	m_pRenderTargetView(nullptr),
	m_pDepthStencilView(nullptr) 
{
	ZeroMemory(&m_ScreenViewPort, sizeof(D3D11_VIEWPORT));
	// ��һ��ȫ��ָ���ȡ����࣬�������ǾͿ�����Windows��Ϣ����Ļص�����
	// �����������ڲ��Ļص�������
	g_pd3dApp = this;
}

D3DApp::~D3DApp()
{
	if (m_pd3dImmediateContext)
	{
		m_pd3dImmediateContext->ClearState();
	}
}

HINSTANCE D3DApp::AppInstance()const
{
	return m_hAppInstance;
}

HWND D3DApp::MainWND() const
{
	return m_hMainWnd;
}

float D3DApp::AspectRatio() const
{
	return static_cast<float>(m_ClientWidth / m_ClientHeight);
}

int D3DApp::Run()
{
	MSG msg = { 0 };

	m_Timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_Timer.Tick();

			if (!m_AppPause)
			{
				CalculateFrameStates();
				UpdateScene(m_Timer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::Init()
{
	m_pMouse = std::make_unique<DirectX::Mouse>();
	m_pKeyboard = std::make_unique<DirectX::Keyboard>();

	if (!InitMainWindow())
		return false;

	if (!InitDirect2D())
		return false;

	if (!InitDirect3D())
		return false;

	return true;
}


LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wparam) == WA_INACTIVE)
		{
			m_AppPause = true;
			m_Timer.Stop();
		}
		else
		{
			m_AppPause = false;
			m_Timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_ClientWidth = LOWORD(lparam);
		m_ClientHeight = HIWORD(lparam);
		if (m_pd3d11Device)
		{
			if (wparam == SIZE_MINIMIZED)
			{
				m_AppPause = true;
				m_AppMinimize = true;
				m_AppMaximize = false;
			}
			else if (wparam == SIZE_MAXIMIZED)
			{
				m_AppPause = false;
				m_AppMinimize = false;
				m_AppMaximize = true;
				OnResize();
			}
			else if (wparam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (m_AppMinimize)
				{
					m_AppPause = false;
					m_AppMinimize = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (m_AppMaximize)
				{
					m_AppPause = false;
					m_AppMaximize = false;
					OnResize();
				}
				else if (m_AppResize)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or m_pSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_AppPause = true;
		m_AppResize = true;
		m_Timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_AppPause = false;
		m_AppResize = false;
		m_Timer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lparam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lparam)->ptMinTrackSize.y = 200;
		return 0;

		// �����Щ����/����¼�
	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		m_pMouse->ProcessMessage(msg, wparam, lparam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_pKeyboard->ProcessMessage(msg, wparam, lparam);
		return 0;

	case WM_ACTIVATEAPP:
		m_pMouse->ProcessMessage(msg, wparam, lparam);
		m_pKeyboard->ProcessMessage(msg, wparam, lparam);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}
	//ʹ��GetSystemMetrics��������ȡ��ǰ��Ļ�ֱ���
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);


	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"D3DWndClassName", m_MainWNDCaption.c_str(),
		WS_OVERLAPPEDWINDOW, (screenWidth-width)/2.0f, (screenHeight-height)/2.0f, width, height, 0, 0, m_hAppInstance, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

bool D3DApp::InitDirect2D()
{
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pd2dFactory.GetAddressOf()));
	HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(m_pdWriteFactory.GetAddressOf())));

	return true;
}

//D3D�豸��D3D�豸�����ĵĴ���
bool D3DApp::InitDirect3D()
{
	HRESULT hr = S_OK;//������һ��HRESULT����,��ʼ����ֵ����Ϊ�����ɹ�

	// ����D3D�豸 �� D3D�豸������
	UINT createDeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;// Direct2D��Ҫ֧��BGRA��ʽ
#if defined(DEBUG)||defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;//a=a|b;����a��b����λ��(|)�����������ֵ��a
#endif
	// ������������
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverType = ARRAYSIZE(driver_types);

	//�����ȼ�����
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};//ѡ��ʹ����һ�ֵȼ���Direc3D�ȼ������Եȼ���֧�����ȡ���ڵ�ǰʹ�õ��Կ���D3D�豸�İ汾ȡ����������ϵͳ
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_FEATURE_LEVEL featureLevel;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverType; driverTypeIndex++)
	{
		d3dDriverType = driver_types[driverTypeIndex];

		hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlag, featureLevels,numFeatureLevels , D3D11_SDK_VERSION, m_pd3d11Device.GetAddressOf()
		,&featureLevel,m_pd3dImmediateContext.GetAddressOf());
		//https://docs.microsoft.com/zh-cn/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice

		if (hr == E_INVALIDARG)//invalid argument. Direct3D 11.0 ��API������D3D_FEATURE_LEVEL_11_1������������Ҫ�������Եȼ�11.0�Լ����µİ汾
		{
			hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlag, &featureLevels[1], numFeatureLevels-1, D3D11_SDK_VERSION, m_pd3d11Device.GetAddressOf()
				, &featureLevel, m_pd3dImmediateContext.GetAddressOf());
		}

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}
	//// ����Ƿ�֧�����Եȼ�11.0��11.1
	if (featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported!.", 0, 0);
		return false;
	}

	m_pd3d11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_4xMSAAQuality);//4��8λ����ɣ�ÿ����ԱΪ[0,1.f]֮��
	assert(m_4xMSAAQuality > 0);//����������������������ش�������ֹ����ִ��

	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> dxgiadapter = nullptr;
	ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;// D3D11.0(����DXGI1.1)�Ľӿ���
	ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;// D3D11.1(����DXGI1.2)���еĽӿ���

	// Ϊ����ȷ���� DXGI������������������Ҫ��ȡ���� D3D�豸 �� DXGI�������������������
	// "IDXGIFactory::CreateSwapChain: This function is being called with a device from a different IDXGIFactory."
	//ComPtr::As() ���ڲ�ѯ����ӿ���ָ��ָ��Ķ����Ƿ�ʵ����ĳ���ض��ӿ�,�����򷵻�����ض��ӿڵ�ָ��,�޾���nullptr
	HR(m_pd3d11Device.As(&dxgiDevice));
	HR(dxgiDevice->GetAdapter(dxgiadapter.GetAddressOf()));
	HR(dxgiadapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf())));

	hr = dxgiFactory1.As(&dxgiFactory2);//�鿴�ö����Ƿ����IDXGIFactory2�ӿ�

	if (dxgiFactory2 != nullptr)// �����������˵��֧��D3D11.1
	{
		HR(m_pd3d11Device.As(&m_pd3d11Device1));
		HR(m_pd3dImmediateContext.As(&m_pd3dImmediateContext1));

		// �����ֽṹ����������������
		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));//��0��ʼ���ڴ�飻
		sd.Width = m_ClientWidth;
		sd.Height = m_ClientHeight;
		sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

		//�Ƿ���ز���
		if (m_Enable4xMSAA)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_4xMSAAQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Count = 0;
		}

		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
		fd.RefreshRate.Numerator=60;//��Ļˢ���ʷ���
		fd.RefreshRate.Denominator = 1;////��Ļˢ���ʷ���
		fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fd.Windowed = TRUE;// �Ƿ񴰿ڻ�

		// Ϊ��ǰ���ڴ���������
		HR(dxgiFactory2->CreateSwapChainForHwnd(m_pd3d11Device.Get(), m_hMainWnd, &sd, &fd, nullptr, m_pSwapChain1.GetAddressOf()));
		HR(m_pSwapChain1.As(&m_pSwapChain));
	}
	else//��֧��D3D11.1,��֧��D3D11.0
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width = m_ClientWidth;
		sd.BufferDesc.Height = m_ClientHeight;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//�Ƿ���ز���
		if (m_Enable4xMSAA)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_4xMSAAQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Count = 0;
		}

		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		sd.Windowed = TRUE;
		sd.OutputWindow = m_hMainWnd;

		HR(dxgiFactory1->CreateSwapChain(m_pd3d11Device.Get(), &sd, m_pSwapChain.GetAddressOf()));

		
	}
	dxgiFactory1->MakeWindowAssociation(m_hMainWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
	/*
	// ���õ��Զ�����
	D3D11SetDebugObjectName(m_pd3dImmediateContext.Get(), "ImmediateContext");
	DXGISetDebugObjectName(m_pSwapChain.Get(), "SwapChain");
	*/
	// ÿ�����ڱ����µ�����С��ʱ�򣬶���Ҫ�������OnResize���������ڵ���
	// �Ա�������ظ�
	OnResize();
	return true;
}


void D3DApp::OnResize()
{
	assert(m_pd3dImmediateContext);
	assert(m_pd3d11Device);
	assert(m_pSwapChain);

	if (m_pd3d11Device!=nullptr)
	{
		assert(m_pd3dImmediateContext1);
		assert(m_pd3d11Device1);
		assert(m_pSwapChain1);
	}

	m_pRenderTargetView.Reset();
	m_pDepthStencilBuffer.Reset();
	m_pDepthStencilView.Reset();


	// ���轻�����������´�����ȾĿ����ͼ
	ComPtr<ID3D11Texture2D> backBuffer;
	HR(m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));// �����������ţ���0��BufferCount - 1,[In]�������Ľӿ�����ID, [Out]��ȡ���Ļ�����
	HR(m_pd3d11Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

	// ���õ��Զ�����
	D3D11SetDebugObjectName(backBuffer.Get(), "BackBuffer[0]");

	backBuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.

	// Ҫʹ�� 4X MSAA? --��Ҫ������������MASS����
	if (m_Enable4xMSAA)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMSAAQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;// ���ݵ�CPU/GPU����Ȩ��
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;// ʹ��D3D11_BIND_FLAGö�������������ݵ�ʹ������
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// ������Ȼ������Լ����ģ����ͼ
	HR(m_pd3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf()));
	HR(m_pd3d11Device->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf()));
	// ����ȾĿ����ͼ�����/ģ�建������ϵ�����
	m_pd3dImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	// �����ӿڱ任
	m_ScreenViewPort.TopLeftX = 0;
	m_ScreenViewPort.TopLeftY = 0;
	m_ScreenViewPort.Width = static_cast<float>(m_ClientWidth);
	m_ScreenViewPort.Height = static_cast<float>(m_ClientHeight);
	m_ScreenViewPort.MaxDepth = 1.0;
	m_ScreenViewPort.MinDepth = 0.0;

	m_pd3dImmediateContext->RSSetViewports(1, &m_ScreenViewPort);
	/*
	// ���õ��Զ�����
	D3D11SetDebugObjectName(m_pDepthStencilBuffer.Get(), "DepthStencilBuffer");
	D3D11SetDebugObjectName(m_pDepthStencilView.Get(), "DepthStencilView");
	D3D11SetDebugObjectName(m_pRenderTargetView.Get(), "BackBufferRTV[0]");
	*/
}



void D3DApp::CalculateFrameStates()
{	// �ô������ÿ��֡�٣�������ÿһ֡��Ⱦ��Ҫ��ʱ�䣬��ʾ�ڴ��ڱ���
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((m_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_MainWNDCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


