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
	//常量缓冲区根据刷新频率和类别来进行更细致的分块，尽可能（但不一定能完全做到）保证每一次更新都不会有变量在进行无意义的刷新
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
		float pad;//16字节打包对其
	};

	//游戏对象类
	class GameObject
	{
	public:
		GameObject();

		//获取物体变换
		Transform& GetTransform();
		//获取物体变换
		const Transform& GetTransform() const;

		//设置缓冲区
		template<class VertexType, class IndexType>
		void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);

		//设置纹理
		void SetTexture(ID3D11ShaderResourceView* texture);

		//绘制
		void Draw(ID3D11DeviceContext* deviceContext);

		// 设置调试对象名
		// 若缓冲区被重新设置，调试对象名也需要被重新设置
		void SetDebugObjectName(const std::string& name);

	private:
		Transform m_Transform;               //物体变换信息
		ComPtr<ID3D11ShaderResourceView>  m_pTexture; //纹理
		ComPtr<ID3D11Buffer> m_pVertexBuffer;          //顶点缓冲区
		ComPtr<ID3D11Buffer> m_pIndexBuffer;           //索引缓冲区
		UINT m_VertexStride;                           //顶点字节大小
		UINT m_IndexCount;                             //索引
	};

	//摄像机模式
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

	ComPtr<ID2D1SolidColorBrush> m_pSolidColorBrush;      //创建单色笔刷
	ComPtr<IDWriteTextFormat> m_pTextFormat;              //文本格式
	ComPtr<IDWriteFont> m_Font;                           //字体

	ComPtr<ID3D11InputLayout> m_pVertexInputLayout2D;     //用于2D的顶点输入布局
	ComPtr<ID3D11InputLayout> m_pVertexInputLayout3D;     //用于3D的顶点输入布局	
	ComPtr<ID3D11Buffer>m_pConstantBuffer[4];            //常量缓冲区

	GameObject   m_WoodCrate;                    //木盒
 	GameObject   m_Floor;                         //地板
	std::vector<GameObject> m_Walls;              //墙

	ComPtr<ID3D11VertexShader> m_pVertexShader2D;          //用于2D的顶点着色器
	ComPtr<ID3D11VertexShader> m_pVertexShader3D;          //用于的3D顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader2D;            //用于2D的像素着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader3D;            //用于3D的像素着色器

	CBChangesEveryDrawing m_CBDrawing;
	CBChangesEveryFrame m_CBFrame;                         // 该缓冲区存放仅在每一帧进行更新的变量
	CBChangesOnresize m_CBOnResize;                        // 该缓冲区存放仅在窗口大小变化时进行更新的变量
	CBChangesRarely m_CBRarely;                            // 该缓冲区存放不会修改的变量

	ComPtr<ID3D11SamplerState> m_pSamplerState;             //采样器状态

	std::shared_ptr<Camera> m_pCamera;                      //摄像机
	CameraMode m_CameraMode;                                //摄像机模式


};

#endif // !GAMEAPP_H
