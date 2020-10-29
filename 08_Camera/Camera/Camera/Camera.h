#include<d3d11_1.h>
#include<DirectXMath.h>
#include"Transform.h"

using namespace DirectX;

class Camera
{
public:
	Camera() = default;
	virtual ~Camera()=0;

	//��ȡ�����λ��
	XMFLOAT3 GetPosition()const;
	XMVECTOR GetPositionXM()const;

	//��ȡ�������תŷ����
	//��ȡ��X����ת��ŷ���ǻ���
	float GetRotationX() const;
	//��ȡ��Y����ת��ŷ���ǻ���
	float GetRotationY() const;


	//��ȡ���������������
	XMFLOAT3 GetRightAxis() const;
	XMFLOAT3 GetUpAxis() const;
	XMFLOAT3 GetForwardAxis() const;

	XMVECTOR GetRightAxisXM() const;
	XMVECTOR GetUpAxisXM() const;
	XMVECTOR GetForwardAxisXM() const;

	//��ȡ���������
	XMMATRIX GetViewXM() const;
	XMMATRIX GetProjXM() const;
	XMMATRIX GetViewProjXM() const;

	//��ȡ�ӿ�
	D3D11_VIEWPORT GetViewPort() const;

	//������׶��
	void SetFrustum(float fovY, float aspect, float ZNear, float ZFar);

	//�����ӿ�
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:

	//������任
	Transform m_Transform = {};

	//��׶����
	float m_FovY = 0.0f;
	float m_Aspect = 0.0f;
	float m_ZNear = 0.0f;
	float m_ZFar = 0.0f;

	//��ǰ�ӿ�
	D3D11_VIEWPORT m_ViewPort = {};

};

//��һ�˳������
class FirstPersonCamera :public Camera
{
public:
	FirstPersonCamera() = default;
	~FirstPersonCamera()override;
	//��д (override) �̳��Ի����Ա������ʵ�� (implementation) ʱ,override �ؼ��֣�������ʽ������������������
	//��Щ��Ա������Ҫ����д�����û����д����������ᱨ��

	//�����������λ��
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//����������ĳ���
	void LookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up);
	void LookTo(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& up);

	//����������ƶ�
	void MoveLeftRight(float distance);

	//�����ǰ���ƶ�����������ܲ������Ϸ�(0,1,0)��
	void MoveForward(float distance);

	//���������ǰ��ƽ�ƣ���������Ϸ�(0,1,0)��
	void WatchForward(float distance);

	//��������¹۲�,rad�����Ͽ���rad�����¿�
	void RotateX(float rad);

	//��������ҹ۲�,rad�����󿴣�rad�����ҿ�
	void RotateY(float rad);

};


//�����˳������
class ThirdPersonCamera :public Camera
{
public:
	ThirdPersonCamera() = default;
	~ThirdPersonCamera() override;

	//��ȡ���������λ��
	XMFLOAT3 GetTargetPosition() const;

	//��ȡ���������ľ���
	float GetDistanceFromTarget() const;

	// �����崹ֱ��תrad(ע����x����תŷ���ǻ���������[0, pi/3])
	void RotateX(float rad);
	//������ˮƽ��תrad
	void RotateY(float rad);
	// ���ó�ʼ��X��Ļ���rad(ע����x����תŷ���ǻ���������[0, pi/3])
	void SetRotateX(float rad);
	//���ó�ʼ��Y��Ļ���rad
	void SetRotateY(float rad);

	//��������
	void Approach(float distance);

	// ���ò��󶨴����������λ��
	void SetTargetPosition(const XMFLOAT3& targetPosition);

	//�����������ʼ����
	void SetDistance(float distance);

	//������С/����������
	void SetDistanceMinMax(float minDist, float maxDist);

private:
	XMFLOAT3 m_TargetPosition = {};//����λ��
	float m_Distance = 0.0f;//���������
	float m_MinDist = 0.0f;
	float m_MaxDist = 0.0f;//�����С�������
};
