#ifndef GAMEAPP_H
#define GAMEAPP_H
#include"D3DApp.h"
#include"DXTrace.h"

class GameAPP :public D3DApp
{
public:
	struct VertexPosColor//�ܹ�����C++�ṹ����HLSL�ṹ��(hlsli)�Ķ�Ӧ��ϵ����Ҫʹ��ID3D11InputLayout���벼��������ÿһ����Ա����;�����塢��С����Ϣ
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		static const D3D11_INPUT_ELEMENT_DESC inputlayout[2];//��̬ȫ�ֱ���������������ṹ����ÿ����Ա�ľ�����Ϣ
	};

public:
	GameAPP(HINSTANCE hInstance);
	~GameAPP();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
private:
	bool InitEffect();
	bool InitResource();

private:
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout;//�������벼��
	ComPtr<ID3D11VertexShader> m_pVertexShader;//������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader;//������ɫ��
	ComPtr<ID3D11Buffer> m_pVertexBuffer;//���㻺����

};






#endif // !GAMEAPP_H
