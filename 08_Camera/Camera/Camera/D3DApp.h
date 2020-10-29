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
	D3DApp(HINSTANCE hInstance);      //Ӧ��ʵ����ʼ�����캯��
	virtual~D3DApp();

	HINSTANCE  AppInstance() const;  //��ȡӦ�ó���ʵ���ľ��
	HWND       MainWND() const;      //������ʵ��
	float      AspectRatio() const;  //���ڿ�߱�

	int Run();                       //������Ϸ������

	virtual bool Init();             //��ʼ�����ں�Direct3D����
	virtual void OnResize();         //�������ڴ�С
	virtual void UpdateScene(float dt) = 0; //��֡����
	virtual void DrawScene() = 0;         //֡����
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);//������Ϣ�ص�����

protected:
	bool InitMainWindow();              //��ʼ������
	bool InitDirect2D();                //��ʼ��D2D
	bool InitDirect3D();               //��ʼ��D3D

	void CalculateFrameStates();//����ÿ��֡����������ʾ

protected:
	HINSTANCE  m_hAppInstance;       //Ӧ��ʵ�����
	HWND       m_hMainWnd;            //�����ھ��
	bool       m_AppPause;            //Ӧ���Ƿ���ͣ
	bool       m_AppMaximize;         //Ӧ���Ƿ����
	bool       m_AppMinimize;         //Ӧ���Ƿ���С��
	bool       m_AppResize;            //���ڴ�С�Ƿ�仯
	bool       m_Enable4xMSAA;          //�Ƿ�����4x���ز���
	UINT       m_4xMSAAQuality;          //4xMSAA֧�ֵ������ȼ�

	
	GameTimer m_Timer;               //��ʱ��

	// ʹ��ģ�����(C++11)��������
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//Direct2D
	ComPtr<ID2D1Factory>             m_pd2dFactory;        //D2D����
	ComPtr<ID2D1RenderTarget>        m_pd2dRenderTarget;   //D2D��ȾĿ��
	ComPtr<IDWriteFactory>           m_pdWriteFactory;     //DWrite����

	//Direct3D 11
	ComPtr<ID3D11Device>             m_pd3d11Device;    //D3D11�豸
	ComPtr<ID3D11DeviceContext>      m_pd3dImmediateContext;//D3D11.1�豸������
	ComPtr<IDXGISwapChain>           m_pSwapChain;       //D3D11������

	//Direct3D 11.1
	ComPtr<ID3D11Device1>            m_pd3d11Device1;      //D3D11.1�豸
	ComPtr<ID3D11DeviceContext1>     m_pd3dImmediateContext1;//D3D111.1�豸������
	ComPtr<IDXGISwapChain1>          m_pSwapChain1;       //D3D11.1������

	//������Դ
	ComPtr<ID3D11Texture2D>          m_pDepthStencilBuffer; //���ģ�建����
	ComPtr<ID3D11RenderTargetView>   m_pRenderTargetView;   //��ȾĿ����ͼ
	ComPtr<ID3D11DepthStencilView>   m_pDepthStencilView;   //���ģ����ͼ
	D3D11_VIEWPORT                   m_ScreenViewPort;      //��Ļ�ӿ�

	//����������
	std::unique_ptr<DirectX::Mouse>  m_pMouse; //���
	DirectX::Mouse::ButtonStateTracker m_MouseTracker;  // ���״̬׷����
	std::unique_ptr<DirectX::Keyboard> m_pKeyboard; //����
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker; //����״̬׷����

	std::wstring   m_MainWNDCaption;        //�����ڱ���
	int m_ClientWidth;                      //�ӿڿ�
	int m_ClientHeight;                     //�ӿڸ�
};

#endif