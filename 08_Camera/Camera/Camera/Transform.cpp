#include"Transform.h"

using namespace DirectX;

Transform::Transform(const XMFLOAT3& scale, const XMFLOAT3& rotation, const XMFLOAT3& position)
	:m_Scale(scale), m_Rotation(rotation), m_Position(position) {}


XMFLOAT3 Transform::GetScale() const
{
	return m_Scale;
}
XMVECTOR Transform::GetScaleXM()const
{
	return XMLoadFloat3(&m_Scale);
}


XMFLOAT3 Transform::GetRotationEuler()const
{
	return m_Rotation;
}
XMVECTOR Transform::GetRotationEulerXM() const
{
	return XMLoadFloat3(&m_Rotation);
}


XMFLOAT3 Transform::GetPosition() const
{
	return m_Position;
}
XMVECTOR Transform::GetPositionXM() const
{
	return XMLoadFloat3(&m_Position);
}


//获取某个方向轴就是旋转矩阵的某一行
//可通过欧拉旋转角获知旋转矩阵
XMFLOAT3 Transform::GetAxisRight() const
{
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMFLOAT3 RightAxis;
	XMStoreFloat3(&RightAxis, RotationMatrix.r[0]);//r:Array of four vectors, representing the rows of the matrix
	return RightAxis;
}
XMVECTOR Transform::GetAxisRightXM() const
{
	XMFLOAT3 RightAxis = GetAxisRight();
	return XMLoadFloat3(&RightAxis);
}


XMFLOAT3 Transform::GetAxisUp() const
{
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMFLOAT3 UpAxis;
	XMStoreFloat3(&UpAxis, RotationMatrix.r[1]);//r:Array of four vectors, representing the rows of the matrix
	return UpAxis;
}
XMVECTOR Transform::GetAxisUpXM() const
{
	XMFLOAT3 UpAxis = GetAxisUp();
	return XMLoadFloat3(&UpAxis);
}


XMFLOAT3 Transform::GetAxisForward() const
{
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMFLOAT3 ForwardAxis;
	XMStoreFloat3(&ForwardAxis, RotationMatrix.r[2]);//r:Array of four vectors, representing the rows of the matrix
	return ForwardAxis;
}
XMVECTOR Transform::GetAxisForwardXM() const
{
	XMFLOAT3 ForwardAxis = GetAxisForward();
	return XMLoadFloat3(&ForwardAxis);
}


XMFLOAT4X4 Transform::GetLocalToWorldMatrix() const
{
	XMFLOAT4X4 World;
	XMStoreFloat4x4(&World, GetLocalToWorldMatrixXM());
	return World;
}
XMMATRIX Transform::GetLocalToWorldMatrixXM() const
{
	XMMATRIX ScalingMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
	XMMATRIX World=ScalingMatrix * RotationMatrix * TranslationMatrix;
	return World;
}


XMFLOAT4X4 Transform::GetWorldToLocalMatrix() const
{
	XMFLOAT4X4 InvWorld;
	XMStoreFloat4x4(&InvWorld, GetWorldToLocalMatrixXM());
	return InvWorld;
}
XMMATRIX Transform::GetWorldToLocalMatrixXM() const
{
	XMMATRIX InvWorld = XMMatrixInverse(nullptr, GetLocalToWorldMatrixXM());
	return InvWorld;
}


void Transform::SetScale(const XMFLOAT3& scale)
{
	m_Scale = scale;
}
void Transform::SetScale(float x, float y, float z)
{
	m_Scale = XMFLOAT3(x, y, z);
}


void Transform::SetRotationEuler(const XMFLOAT3& eulerAnglesInRadian)
{
	m_Rotation = eulerAnglesInRadian;
}
void Transform::SetRotationEuler(float roll, float pitch, float yaw)
{
	m_Rotation = XMFLOAT3(roll, pitch, yaw);
}


void Transform::SetPosition(const XMFLOAT3& position)
{
	m_Position = position;
}
void Transform::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
}


void Transform::Rotate(const XMFLOAT3& eulerAnglesInRadian)
{
	XMVECTOR newRotation = XMVectorAdd(XMLoadFloat3(&m_Rotation), XMLoadFloat3(&eulerAnglesInRadian));//两个vector加起来
	XMStoreFloat3(&m_Rotation, newRotation);
}

void Transform::RotateAxis(const XMFLOAT3& axis, float radian)
{
	XMMATRIX newRotationMatrixXM = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation)) * XMMatrixRotationAxis(XMLoadFloat3(&axis), radian);//两个旋转矩阵相乘
	XMFLOAT4X4 newRotationMatrix;
	XMStoreFloat4x4(&newRotationMatrix, newRotationMatrixXM);
	m_Rotation=GetEulerAnglesFromRotationMatrix(newRotationMatrix);
}

void Transform::RotatePointAxis(const XMFLOAT3& point, const XMFLOAT3& axis, float radian)
{
	XMVECTOR rotationVec = XMLoadFloat3(&m_Rotation);
	XMVECTOR positionVec = XMLoadFloat3(&m_Position);
	XMVECTOR pointVec = XMLoadFloat3(&point);

	XMMATRIX RotationMatrix;
	//绕某个点旋转之前，先平移到物体原先位置做完旋转，再平移回去
	RotationMatrix = XMMatrixTranslationFromVector(positionVec - pointVec);
	RotationMatrix *= XMMatrixRotationRollPitchYawFromVector(rotationVec) * XMMatrixRotationAxis(XMLoadFloat3(&axis), radian);
	RotationMatrix *= XMMatrixTranslationFromVector(pointVec);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, RotationMatrix);
	m_Rotation=GetEulerAnglesFromRotationMatrix(R);

	XMStoreFloat3(&m_Position, RotationMatrix.r[3]);

}



void Transform::Translate(const XMFLOAT3& direction, float length)
{
	XMVECTOR directionNormalized = XMVector3Normalize(XMLoadFloat3(&direction));
	XMVECTOR newPosition = directionNormalized * length + XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, newPosition);
}



void Transform::LookAt(const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMMATRIX View = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&target), XMLoadFloat3(&up));//世界坐标系下摄像机视野矩阵(local->world)
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);//(world->local)物体在世界坐标系->物体在相机坐标系
	XMFLOAT4X4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, InvView);
	m_Rotation = GetEulerAnglesFromRotationMatrix(RotationMatrix);
}


void Transform::LookTo(const XMFLOAT3& direction, const XMFLOAT3& up)
{
	XMMATRIX View = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&direction), XMLoadFloat3(&up));//世界坐标系下摄像机视野矩阵(local->world)
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);//(world->local)物体在世界坐标系->物体在相机坐标系
	XMFLOAT4X4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, InvView);
	m_Rotation = GetEulerAnglesFromRotationMatrix(RotationMatrix);
}


XMFLOAT3 Transform::GetEulerAnglesFromRotationMatrix(const XMFLOAT4X4& rotationMatrix)
{
	// 通过旋转矩阵反求欧拉角
	float c = sqrtf(1.0f - rotationMatrix._32 * rotationMatrix._32);
	// 防止rotationMatrix._32出现大于1的情况
	if (rotationMatrix._32 > 1.0f)
		c = 0.0f;
	XMFLOAT3 rotation;
	rotation.x = atan2f(-rotationMatrix._32, c);
	rotation.y = atan2f(rotationMatrix._31, rotationMatrix._33);
	rotation.z = atan2f(rotationMatrix._12, rotationMatrix._22);
	return rotation;
}