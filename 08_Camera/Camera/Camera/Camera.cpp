
#include"Camera.h"

using namespace DirectX;



Camera::~Camera(){}


XMFLOAT3 Camera::GetPosition() const
{
	return m_Transform.GetPosition();
}
XMVECTOR Camera::GetPositionXM() const
{
	return m_Transform.GetPositionXM();
}



float Camera::GetRotationX() const
{
	return m_Transform.GetRotationEuler().x;
}
float Camera::GetRotationY() const
{
	return m_Transform.GetRotationEuler().y;
}


XMFLOAT3 Camera::GetRightAxis() const
{
	return m_Transform.GetAxisRight();
}
XMFLOAT3 Camera::GetUpAxis() const
{
	return m_Transform.GetAxisUp();
}
XMFLOAT3 Camera::GetForwardAxis() const
{
	return m_Transform.GetAxisForward();
}
XMVECTOR Camera::GetRightAxisXM() const
{
	return m_Transform.GetAxisRightXM();
}
XMVECTOR Camera::GetUpAxisXM() const
{
	return m_Transform.GetAxisUpXM();
}
XMVECTOR Camera::GetForwardAxisXM() const
{
	return m_Transform.GetAxisForwardXM();
}


XMMATRIX Camera::GetViewXM() const
{
	return m_Transform.GetWorldToLocalMatrixXM();
}
XMMATRIX Camera::GetProjXM() const
{
	return XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_ZNear, m_ZFar);
}
XMMATRIX Camera::GetViewProjXM() const
{
	return GetViewXM() * GetProjXM();
}


D3D11_VIEWPORT Camera::GetViewPort() const
{
	return m_ViewPort;
}


void Camera::SetFrustum(float fovY, float aspect, float ZNear, float ZFar)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_ZNear = ZNear;
	m_ZFar = ZFar;
}


void Camera::SetViewPort(const D3D11_VIEWPORT& viewPort)
{
	m_ViewPort = viewPort;
}
void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_ViewPort.TopLeftX = topLeftX;
	m_ViewPort.TopLeftY = topLeftY;
	m_ViewPort.Width = width;
	m_ViewPort.Height = height;
	m_ViewPort.MinDepth = minDepth;
	m_ViewPort.MaxDepth = maxDepth;
}



// 第一人称/自由视角摄像机
//
FirstPersonCamera::~FirstPersonCamera(){}


void FirstPersonCamera::SetPosition(const XMFLOAT3& position)
{
	m_Transform.SetPosition(position);
}
void FirstPersonCamera::SetPosition(float x, float y, float z)
{
	m_Transform.SetPosition(x, y, z);
}


void FirstPersonCamera::LookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up)
{
	m_Transform.SetPosition(position);
	m_Transform.LookAt(target, up);
}
void FirstPersonCamera::LookTo(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& up)
{
	m_Transform.SetPosition(position);
	m_Transform.LookTo(direction, up);
}


void FirstPersonCamera::MoveLeftRight(float distance)
{
	m_Transform.Translate(m_Transform.GetAxisRight(), distance);
}
void FirstPersonCamera::MoveForward(float distance)
{
	m_Transform.Translate(m_Transform.GetAxisForward(), distance);
}
void FirstPersonCamera::WatchForward(float distance)
{
	XMVECTOR rightVec = m_Transform.GetAxisRightXM();
	XMVECTOR frontVec = XMVector3Normalize(XMVector3Cross(rightVec,g_XMIdentityR1));//DirectX叉乘左手系
	XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_Transform.Translate(front, distance);
}


void FirstPersonCamera::RotateX(float rad)
{
	XMFLOAT3 rotationEulerAngle = m_Transform.GetRotationEuler();
	// 将绕x轴旋转弧度限制在[-7pi/18, 7pi/18]之间
	rotationEulerAngle.x += rad;
	if (rotationEulerAngle.x > 7.0f * XM_PI / 18.0f)
	{
		rotationEulerAngle.x = 7.0f * XM_PI / 18.0f;
	}
	else if (rotationEulerAngle.x < -7.0f * XM_PI / 18.0f)
	{
		rotationEulerAngle.x = -7.0f * XM_PI / 18.0f;
	}
	return m_Transform.SetRotationEuler(rotationEulerAngle);
}
void FirstPersonCamera::RotateY(float rad)
{
	XMFLOAT3 rotationEulerAngle = m_Transform.GetRotationEuler();
	rotationEulerAngle.y = XMScalarModAngle(rotationEulerAngle.y + rad);//Modulo the range of the given angle such that -XM_PI <= Angle < XM_PI
	return m_Transform.SetRotationEuler(rotationEulerAngle);
}



//第三人称摄像机
ThirdPersonCamera::~ThirdPersonCamera() {};

XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return m_TargetPosition;
}

float ThirdPersonCamera::GetDistanceFromTarget() const
{
	return m_Distance;
}

void ThirdPersonCamera::RotateX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.x += rad;
	// 将绕x轴旋转弧度限制在[0, pi/3]之间
	if (rotation.x > XM_PI / 3.0f)
	{
		rotation.x = XM_PI / 3.0f;
	}
	else if (rotation.x < 0.0f)
	{
		rotation.x = 0.0f;
	}
	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::RotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.y += rad;
	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::SetRotateX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.x += rad;
	if (rotation.x < 0.0f)
		rotation.x = 0.0f;
	else if (rotation.x > XM_PI / 3)
		rotation.x = XM_PI / 3;
	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::SetRotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.y = XMScalarModAngle(rotation.y+rad);
	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::Approach(float distance)
{
	m_Distance += distance;
	// 限制距离在[m_MinDist, m_MaxDist]之间
	if (m_Distance > m_MaxDist)
	{
		m_Distance = m_MaxDist;
	}
	else if (m_Distance < m_MinDist)
	{
		m_Distance = m_MinDist;
	}
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::SetTargetPosition(const XMFLOAT3& targetPosition)
{
	m_TargetPosition = targetPosition;
}

void ThirdPersonCamera::SetDistance(float distance)
{
	m_Distance = distance;
}

void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	m_MinDist = minDist;
	m_MaxDist = maxDist;
}
/*

#include "Camera.h"
using namespace DirectX;

Camera::~Camera()
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return m_Transform.GetPositionXM();
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_Transform.GetPosition();
}

float Camera::GetRotationX() const
{
	return m_Transform.GetRotationEuler().x;
}

float Camera::GetRotationY() const
{
	return m_Transform.GetRotationEuler().y;
}


XMVECTOR Camera::GetRightAxisXM() const
{
	return m_Transform.GetAxisRightXM();
}

XMFLOAT3 Camera::GetRightAxis() const
{
	return m_Transform.GetAxisRight();
}

XMVECTOR Camera::GetUpAxisXM() const
{
	return m_Transform.GetAxisUpXM();
}

XMFLOAT3 Camera::GetUpAxis() const
{
	return m_Transform.GetAxisUp();
}

XMVECTOR Camera::GetForwardAxisXM() const
{
	return m_Transform.GetAxisForwardXM();
}

XMFLOAT3 Camera::GetForwardAxis() const
{
	return m_Transform.GetAxisForward();
}

XMMATRIX Camera::GetViewXM() const
{
	return m_Transform.GetWorldToLocalMatrixXM();
}

XMMATRIX Camera::GetProjXM() const
{
	return XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_ZNear, m_ZFar);
}

XMMATRIX Camera::GetViewProjXM() const
{
	return GetViewXM() * GetProjXM();
}

D3D11_VIEWPORT Camera::GetViewPort() const
{
	return m_ViewPort;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_ZNear = nearZ;
	m_ZFar = farZ;
}

void Camera::SetViewPort(const D3D11_VIEWPORT& viewPort)
{
	m_ViewPort = viewPort;
}


void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_ViewPort.TopLeftX = topLeftX;
	m_ViewPort.TopLeftY = topLeftY;
	m_ViewPort.Width = width;
	m_ViewPort.Height = height;
	m_ViewPort.MinDepth = minDepth;
	m_ViewPort.MaxDepth = maxDepth;
}


// ******************
// 第一人称/自由视角摄像机
//

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void FirstPersonCamera::SetPosition(const XMFLOAT3& pos)
{
	m_Transform.SetPosition(pos);
}

void FirstPersonCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	m_Transform.SetPosition(pos);
	m_Transform.LookAt(target, up);
}

void FirstPersonCamera::LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up)
{
	m_Transform.SetPosition(pos);
	m_Transform.LookTo(to, up);
}

void FirstPersonCamera::MoveLeftRight(float d)
{
	m_Transform.Translate(m_Transform.GetAxisRight(), d);
}

void FirstPersonCamera::WatchForward(float d)
{
	XMVECTOR rightVec = m_Transform.GetAxisRightXM();
	XMVECTOR frontVec = XMVector3Normalize(XMVector3Cross(rightVec, g_XMIdentityR1));
	XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_Transform.Translate(front, d);
}

void FirstPersonCamera::MoveForward(float d)
{
	m_Transform.Translate(m_Transform.GetAxisForward(), d);
}

void FirstPersonCamera::RotateX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	// 将绕x轴旋转弧度限制在[-7pi/18, 7pi/18]之间
	rotation.x += rad;
	if (rotation.x > XM_PI * 7 / 18)
		rotation.x = XM_PI * 7 / 18;
	else if (rotation.x < -XM_PI * 7 / 18)
		rotation.x = -XM_PI * 7 / 18;

	m_Transform.SetRotationEuler(rotation);
}

void FirstPersonCamera::RotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.y = XMScalarModAngle(rotation.y + rad);
	m_Transform.SetRotationEuler(rotation);
}



// ******************
// 第三人称摄像机
//

ThirdPersonCamera::~ThirdPersonCamera()
{
}

XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return m_TargetPosition;
}

float ThirdPersonCamera::GetDistanceFromTarget() const
{
	return m_Distance;
}

void ThirdPersonCamera::RotateX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	// 将绕x轴旋转弧度限制在[0, pi/3]之间
	rotation.x += rad;
	if (rotation.x < 0.0f)
		rotation.x = 0.0f;
	else if (rotation.x > XM_PI / 3)
		rotation.x = XM_PI / 3;

	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::RotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.y = XMScalarModAngle(rotation.y + rad);

	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::Approach(float dist)
{
	m_Distance += dist;
	// 限制距离在[m_MinDist, m_MaxDist]之间
	if (m_Distance < m_MinDist)
		m_Distance = m_MinDist;
	else if (m_Distance > m_MaxDist)
		m_Distance = m_MaxDist;

	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::SetRotateX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	// 将绕x轴旋转弧度限制在[0, pi/3]之间
	rotation.x = rad;
	if (rotation.x < 0.0f)
		rotation.x = 0.0f;
	else if (rotation.x > XM_PI / 3)
		rotation.x = XM_PI / 3;

	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::SetRotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotationEuler();
	rotation.y = XMScalarModAngle(rad);
	m_Transform.SetRotationEuler(rotation);
	m_Transform.SetPosition(m_TargetPosition);
	m_Transform.Translate(m_Transform.GetAxisForward(), -m_Distance);
}

void ThirdPersonCamera::SetTargetPosition(const XMFLOAT3& target)
{
	m_TargetPosition = target;
}

void ThirdPersonCamera::SetDistance(float dist)
{
	m_Distance = dist;
}

void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	m_MinDist = minDist;
	m_MaxDist = maxDist;
}

*/