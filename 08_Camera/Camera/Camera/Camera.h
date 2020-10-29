#include<d3d11_1.h>
#include<DirectXMath.h>
#include"Transform.h"

using namespace DirectX;

class Camera
{
public:
	Camera() = default;
	virtual ~Camera()=0;

	//获取摄像机位置
	XMFLOAT3 GetPosition()const;
	XMVECTOR GetPositionXM()const;

	//获取摄像机旋转欧拉角
	//获取绕X轴旋转的欧拉角弧度
	float GetRotationX() const;
	//获取绕Y轴旋转的欧拉角弧度
	float GetRotationY() const;


	//获取摄像机坐标轴向量
	XMFLOAT3 GetRightAxis() const;
	XMFLOAT3 GetUpAxis() const;
	XMFLOAT3 GetForwardAxis() const;

	XMVECTOR GetRightAxisXM() const;
	XMVECTOR GetUpAxisXM() const;
	XMVECTOR GetForwardAxisXM() const;

	//获取摄像机矩阵
	XMMATRIX GetViewXM() const;
	XMMATRIX GetProjXM() const;
	XMMATRIX GetViewProjXM() const;

	//获取视口
	D3D11_VIEWPORT GetViewPort() const;

	//设置视锥体
	void SetFrustum(float fovY, float aspect, float ZNear, float ZFar);

	//设置视口
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:

	//摄像机变换
	Transform m_Transform = {};

	//视锥属性
	float m_FovY = 0.0f;
	float m_Aspect = 0.0f;
	float m_ZNear = 0.0f;
	float m_ZFar = 0.0f;

	//当前视口
	D3D11_VIEWPORT m_ViewPort = {};

};

//第一人称摄像机
class FirstPersonCamera :public Camera
{
public:
	FirstPersonCamera() = default;
	~FirstPersonCamera()override;
	//重写 (override) 继承自基类成员函数的实现 (implementation) 时,override 关键字，可以显式的在派生类中声明，
	//哪些成员函数需要被重写，如果没被重写，则编译器会报错。

	//设置摄像机的位置
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//设置摄像机的朝向
	void LookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up);
	void LookTo(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& up);

	//摄像机左右移动
	void MoveLeftRight(float distance);

	//摄像机前后移动（摄像机可能不是正上方(0,1,0)）
	void MoveForward(float distance);

	//摄像机绝对前后平移（摄像机正上方(0,1,0)）
	void WatchForward(float distance);

	//摄像机上下观察,rad正向上看，rad负向下看
	void RotateX(float rad);

	//摄像机左右观察,rad正向左看，rad负向右看
	void RotateY(float rad);

};


//第三人称摄像机
class ThirdPersonCamera :public Camera
{
public:
	ThirdPersonCamera() = default;
	~ThirdPersonCamera() override;

	//获取跟踪物体的位置
	XMFLOAT3 GetTargetPosition() const;

	//获取与跟踪物体的距离
	float GetDistanceFromTarget() const;

	// 绕物体垂直旋转rad(注意绕x轴旋转欧拉角弧度限制在[0, pi/3])
	void RotateX(float rad);
	//绕物体水平旋转rad
	void RotateY(float rad);
	// 设置初始绕X轴的弧度rad(注意绕x轴旋转欧拉角弧度限制在[0, pi/3])
	void SetRotateX(float rad);
	//设置初始绕Y轴的弧度rad
	void SetRotateY(float rad);

	//拉近物体
	void Approach(float distance);

	// 设置并绑定待跟踪物体的位置
	void SetTargetPosition(const XMFLOAT3& targetPosition);

	//设置与物体初始距离
	void SetDistance(float distance);

	//设置最小/最大允许距离
	void SetDistanceMinMax(float minDist, float maxDist);

private:
	XMFLOAT3 m_TargetPosition = {};//物体位置
	float m_Distance = 0.0f;//与物体距离
	float m_MinDist = 0.0f;
	float m_MaxDist = 0.0f;//最大最小允许距离
};
