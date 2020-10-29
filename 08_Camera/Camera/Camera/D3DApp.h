#ifndef D3DAPP_H
#define D3DAPP_H

#include <wrl/client.h>
#include <string>
#include <d3d11_1.h>
#include<d2d1.h>
#include<dwrite.h>
#include <DirectXMath.h>
#include "GameTimer.h"
#include"Mouse.h"
#include"Keyboard.h"
class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);      //应用实例初始化构造函数
	virtual~D3DApp();

	HINSTANCE  AppInstance() const;  //获取应用程序实例的句柄
	HWND       MainWND() const;      //主窗口实例
	float      AspectRatio() const;  //窗口宽高比

	int Run();                       //运行游戏主程序

	virtual bool Init();             //初始化窗口和Direct3D程序
	virtual void OnResize();         //调整窗口大小
	virtual void UpdateScene(float dt) = 0; //逐帧更新
	virtual void DrawScene() = 0;         //帧绘制
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);//窗口消息回调函数

protected:
	bool InitMainWindow();              //初始化窗口
	bool InitDirect2D();                //初始化D2D
	bool InitDirect3D();               //初始化D3D

	void CalculateFrameStates();//计算每秒帧数并窗口显示

protected:
	HINSTANCE  m_hAppInstance;       //应用实例句柄
	HWND       m_hMainWnd;            //主窗口句柄
	bool       m_AppPause;            //应用是否暂停
	bool       m_AppMaximize;         //应用是否最大化
	bool       m_AppMinimize;         //应用是否最小化
	bool       m_AppResize;            //窗口大小是否变化
	bool       m_Enable4xMSAA;          //是否允许4x多重采样
	UINT       m_4xMSAAQuality;          //4xMSAA支持的质量等级

	
	GameTimer m_Timer;               //计时器

	// 使用模板别名(C++11)简化类型名
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//Direct2D
	ComPtr<ID2D1Factory>             m_pd2dFactory;        //D2D工厂
	ComPtr<ID2D1RenderTarget>        m_pd2dRenderTarget;   //D2D渲染目标
	ComPtr<IDWriteFactory>           m_pdWriteFactory;     //DWrite工厂

	//Direct3D 11
	ComPtr<ID3D11Device>             m_pd3d11Device;    //D3D11设备
	ComPtr<ID3D11DeviceContext>      m_pd3dImmediateContext;//D3D11.1设备上下文
	ComPtr<IDXGISwapChain>           m_pSwapChain;       //D3D11交换链

	//Direct3D 11.1
	ComPtr<ID3D11Device1>            m_pd3d11Device1;      //D3D11.1设备
	ComPtr<ID3D11DeviceContext1>     m_pd3dImmediateContext1;//D3D111.1设备上下文
	ComPtr<IDXGISwapChain1>          m_pSwapChain1;       //D3D11.1交换链

	//常用资源
	ComPtr<ID3D11Texture2D>          m_pDepthStencilBuffer; //深度模板缓冲区
	ComPtr<ID3D11RenderTargetView>   m_pRenderTargetView;   //渲染目标视图
	ComPtr<ID3D11DepthStencilView>   m_pDepthStencilView;   //深度模板视图
	D3D11_VIEWPORT                   m_ScreenViewPort;      //屏幕视口

	//鼠标键盘输入
	std::unique_ptr<DirectX::Mouse>  m_pMouse; //鼠标
	DirectX::Mouse::ButtonStateTracker m_MouseTracker;  // 鼠标状态追踪器
	std::unique_ptr<DirectX::Keyboard> m_pKeyboard; //键盘
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker; //键盘状态追踪器

	std::wstring   m_MainWNDCaption;        //主窗口标题
	int m_ClientWidth;                      //视口宽
	int m_ClientHeight;                     //视口高
};

#endif