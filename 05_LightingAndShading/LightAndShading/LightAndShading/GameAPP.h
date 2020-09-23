#ifndef GAMEAPP_H
#define GAMEAPP_H
#include"D3DApp.h"
#include"DXTrace.h"
#include"Geometry.h"
#include"LightType.h"

class GameAPP :public D3DApp
{
public:
	struct VSConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX worldInvTranspose;
	};
		
	struct PSConstantBuffer
	{
		Material material;
		DirectionalLight directionalLight;
		PointLight pointLight;
		SpotLight spotLight;
		DirectX::XMFLOAT4 eyePos;
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
	bool ResetMesh(const Geometry::MeshData<VertexPosNormalColor, DWORD>& meshData);//��������Ҫʹ�õ�ģ��

private:
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout;      //�������벼��	
	ComPtr<ID3D11Buffer> m_pVertexBuffer;                //���㻺����
	ComPtr<ID3D11Buffer>m_pIndexBuffer;                  //����������
	ComPtr<ID3D11Buffer>m_pConstantBuffer[2];            //����������
	UINT m_indexCount;                                   //���������������С


	ComPtr<ID3D11VertexShader> m_pVertexShader;          //������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader;            //������ɫ��
	VSConstantBuffer m_VSConstantBuffer;                 //�����޸�VS��GPU�����������ı���
	PSConstantBuffer m_PSConstantBuffer;                 //�����޸�PS��GPU�����������ı���

	//���ֹ�Դ
	DirectionalLight m_DirLight;
	PointLight m_PointLight;
	SpotLight m_SpotLight;

	ComPtr<ID3D11RasterizerState> m_pRSWireframe;        //�����դ���Ļ����߿�״̬
	bool m_IsWireFrameMode;                              //��ǰ�Ƿ�����߿�

};






#endif // !GAMEAPP_H
