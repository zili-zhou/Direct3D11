#ifndef TRANSFORM_H
#define TRANSFORM_H

#include<DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform() = default;
	//将缩放、旋转和平移部分拆开来分别更新变换
	Transform(const XMFLOAT3& scale, const XMFLOAT3& rotation, const XMFLOAT3& position);
	~Transform()= default;


	Transform(const Transform&) = default;
	Transform& operator=(const Transform&) = default;

	Transform(Transform&&) = default;
	Transform& operator=(Transform&&) = default;

	//获取对象缩放比例
	XMFLOAT3 GetScale()const;
	XMVECTOR GetScaleXM()const;

	//获取对象的欧拉角（弧度制），对象以Z-X-Y轴顺序旋转
	XMFLOAT3 GetRotationEuler() const;
	XMVECTOR GetRotationEulerXM() const;

	//获取对象位置
	XMFLOAT3 GetPosition() const;
	XMVECTOR GetPositionXM() const;

	//获取右方向轴
	XMFLOAT3 GetAxisRight() const;
	XMVECTOR GetAxisRightXM() const;

	//获取上方向轴
	XMFLOAT3 GetAxisUp() const;
	XMVECTOR GetAxisUpXM() const;

	//获取前方向轴
	XMFLOAT3 GetAxisForward() const;
	XMVECTOR GetAxisForwardXM() const;

	//获取世界变换矩阵
	XMFLOAT4X4 GetLocalToWorldMatrix() const;
	XMMATRIX GetLocalToWorldMatrixXM() const;

	//获取逆世界变换矩阵
	XMFLOAT4X4 GetWorldToLocalMatrix() const;
	XMMATRIX GetWorldToLocalMatrixXM() const;


	//设置对象缩放比例
	void SetScale(const XMFLOAT3& scale);
	void SetScale(float x,float y,float z);

	//设置对象的欧拉角（弧度制），对象以Z-X-Y轴顺序旋转
	void SetRotationEuler(const XMFLOAT3& eulerAnglesInRadian);
	void SetRotationEuler(float roll, float pitch, float yaw);

	//设置对象位置
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//在原有旋转基础上指定旋转欧拉角(更新欧拉角)
	void Rotate(const XMFLOAT3& eulerAnglesInRadian);
	//以原点为中心绕轴旋转（更新欧拉角）
	void RotateAxis(const XMFLOAT3& axis, float radian);
	///以指定点point为中心绕轴旋转
	void RotatePointAxis(const XMFLOAT3& point, const XMFLOAT3& axis, float radian);

	//沿某个方向平移
	void Translate(const XMFLOAT3& direction, float length);

	//观察某一点,求如何由世界坐标系转换到相机坐标系旋转的欧拉角
	void LookAt(const XMFLOAT3& target, const XMFLOAT3& up = { 0.0f, 1.0f , 0.0f });

	//沿某个方向观察
	void LookTo(const XMFLOAT3& direction, const XMFLOAT3& up = { 0.0f, 1.0f , 0.0f });

private:
	//从旋转矩阵获取旋转欧拉角赋值给m_Rotation
	XMFLOAT3 GetEulerAnglesFromRotationMatrix(const XMFLOAT4X4& rotationMatrix);

private:
	XMFLOAT3 m_Scale = { 1.0f,1.0f,1.0f }; //缩放
	XMFLOAT3 m_Rotation = {}; //世界坐标原点旋转欧拉角(按照x,y,z赋值)
	XMFLOAT3 m_Position = {}; //世界坐标中位置


};


#endif
