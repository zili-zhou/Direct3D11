#ifndef GAMEAPP_H
#define GAMEAPP_H
#include"D3DApp.h"
#include"DXTrace.h"
#include"Geometry.h"
#include"LightType.h"
#include"Camera.h"

class GameAPP :public D3DApp
{
public:
	//��������������ˢ��Ƶ�ʺ���������и�ϸ�µķֿ飬�����ܣ�����һ������ȫ��������֤ÿһ�θ��¶������б����ڽ����������ˢ��
	struct CBChangesEveryDrawing
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
	};

	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX view;
		DirectX::XMFLOAT4 eyePos;
	};

	struct CBChangesOnresize
	{
		DirectX::XMMATRIX projection;
	};
			
	struct CBChangesRarely
	{
		
		DirectionalLight directionalLight[10];
		PointLight pointLight[10];
		SpotLight spotLight[10];
		Material material;
		int NumDirectionalLight;
		int NumPointLight;
		int NumSpotLight;
		float pad;//16�ֽڴ������
	};

	//��Ϸ������
	class GameObject
	{
	public:
		GameObject();

		//��ȡ����任
		Transform& GetTransform();
		//��ȡ����任
		const Transform& GetTransform() const;

		//���û�����
		template<class VertexType, class IndexType>
		void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);

		//��������
		void SetTexture(ID3D11ShaderResourceView* texture);

		//����
		void Draw(ID3D11DeviceContext* deviceContext);

		// ���õ��Զ�����
		// �����������������ã����Զ�����Ҳ��Ҫ����������
		void SetDebugObjectName(const std::string& name);

	private:
		Transform m_Transform;               //����任��Ϣ
		ComPtr<ID3D11ShaderResourceView>  m_pTexture; //����
		ComPtr<ID3D11Buffer> m_pVertexBuffer;          //���㻺����
		ComPtr<ID3D11Buffer> m_pIndexBuffer;           //����������
		UINT m_VertexStride;                           //�����ֽڴ�С
		UINT m_IndexCount;                             //����
	};

	//�����ģʽ
	enum class CameraMode { FirstPersonCamera , ThirdPersonCamera, Free };

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

	ComPtr<ID2D1SolidColorBrush> m_pSolidColorBrush;      //������ɫ��ˢ
	ComPtr<IDWriteTextFormat> m_pTextFormat;              //�ı���ʽ
	ComPtr<IDWriteFont> m_Font;                           //����

	ComPtr<ID3D11InputLayout> m_pVertexInputLayout2D;     //����2D�Ķ������벼��
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout3D;     //����3D�Ķ������벼��	
	ComPtr<ID3D11Buffer>m_pConstantBuffer[4];            //����������

	GameObject   m_WoodCrate;                    //ľ��
 	GameObject   m_Floor;                         //�ذ�
	std::vector<GameObject> m_Walls;              //ǽ

	ComPtr<ID3D11VertexShader> m_pVertexShader2D;          //����2D�Ķ�����ɫ��
	ComPtr<ID3D11VertexShader> m_pVertexShader3D;          //���ڵ�3D������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader2D;            //����2D��������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader3D;            //����3D��������ɫ��

	CBChangesEveryDrawing m_CBDrawing;
	CBChangesEveryFrame m_CBFrame;                         // �û�������Ž���ÿһ֡���и��µı���
	CBChangesOnresize m_CBOnResize;                        // �û�������Ž��ڴ��ڴ�С�仯ʱ���и��µı���
	CBChangesRarely m_CBRarely;                            // �û�������Ų����޸ĵı���

	ComPtr<ID3D11SamplerState> m_pSamplerState;             //������״̬

	std::shared_ptr<Camera> m_pCamera;                      //�����
	CameraMode m_CameraMode;                                //�����ģʽ


};

#endif // !GAMEAPP_H
