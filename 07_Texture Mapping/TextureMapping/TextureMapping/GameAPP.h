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
		DirectionalLight directionalLight[10];
		PointLight pointLight[10];
		SpotLight spotLight[10];

		int NumDirectionalLight;
		int NumPointLight;
		int NumSpotLight;
		float pad;//16�ֽڴ������

		DirectX::XMFLOAT4 eyePos;
	};

	enum class ShowMode {WoodCreate, FireCreate, FireAnimation};

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

	template<class VertexType>
	bool ResetMesh(const Geometry::MeshData<VertexType, DWORD>& meshData);//��������Ҫʹ�õ�ģ��

private:

	ComPtr<ID2D1SolidColorBrush> m_pSolidColorBrush;      //������ɫ��ˢ
	ComPtr<IDWriteTextFormat> m_pTextFormat;              //�ı���ʽ

	ComPtr<ID3D11InputLayout> m_pVertexInputLayout2D;     //����2D�Ķ������벼��
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout3D;     //����3D�Ķ������벼��	
	ComPtr<ID3D11Buffer> m_pVertexBuffer;                //���㻺����
	ComPtr<ID3D11Buffer>m_pIndexBuffer;                  //����������
	ComPtr<ID3D11Buffer>m_pConstantBuffer[2];            //����������
	UINT m_indexCount;                                   //���������������С
	int m_CurrFrame;                                       //��ǰ���ŵ��ڼ�֡
	ShowMode m_CurrMode;                                   //��ǰ��ʾģʽ

	//��������ֱ�Ӱ󶨵���ɫ���У���ҪΪ��������Ӧ����ɫ����Դ��ͼ���ܹ�����ɫ��ʹ��
	ComPtr<ID3D11ShaderResourceView> m_pWoodCreate;           //ľ������
	ComPtr<ID3D11ShaderResourceView> m_pFireCreate;            //����ľ��
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_pFireAnim; //������������
	ComPtr<ID3D11SamplerState> m_pSamplerState;          //������״̬

	ComPtr<ID3D11VertexShader> m_pVertexShader2D;          //����2D�Ķ�����ɫ��
	ComPtr<ID3D11VertexShader> m_pVertexShader3D;          //����3D������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader2D;            //����2D��������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader3D;            //����3D��������ɫ��

	VSConstantBuffer m_VSConstantBuffer;                 //�����޸�VS��GPU�����������ı���
	PSConstantBuffer m_PSConstantBuffer;                 //�����޸�PS��GPU�����������ı���

	//���ֹ�Դ
	DirectionalLight m_DirLight;
	PointLight m_PointLight;
	SpotLight m_SpotLight;
	/*
	ComPtr<ID3D11RasterizerState> m_pRSWireframe;        //�����դ���Ļ����߿�״̬
	bool m_IsWireFrameMode;                              //��ǰ�Ƿ�����߿�
	*/
};






#endif // !GAMEAPP_H
