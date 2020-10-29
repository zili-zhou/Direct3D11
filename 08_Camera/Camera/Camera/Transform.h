#ifndef TRANSFORM_H
#define TRANSFORM_H

#include<DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform() = default;
	//�����š���ת��ƽ�Ʋ��ֲ����ֱ���±任
	Transform(const XMFLOAT3& scale, const XMFLOAT3& rotation, const XMFLOAT3& position);
	~Transform()= default;


	Transform(const Transform&) = default;
	Transform& operator=(const Transform&) = default;

	Transform(Transform&&) = default;
	Transform& operator=(Transform&&) = default;

	//��ȡ�������ű���
	XMFLOAT3 GetScale()const;
	XMVECTOR GetScaleXM()const;

	//��ȡ�����ŷ���ǣ������ƣ���������Z-X-Y��˳����ת
	XMFLOAT3 GetRotationEuler() const;
	XMVECTOR GetRotationEulerXM() const;

	//��ȡ����λ��
	XMFLOAT3 GetPosition() const;
	XMVECTOR GetPositionXM() const;

	//��ȡ�ҷ�����
	XMFLOAT3 GetAxisRight() const;
	XMVECTOR GetAxisRightXM() const;

	//��ȡ�Ϸ�����
	XMFLOAT3 GetAxisUp() const;
	XMVECTOR GetAxisUpXM() const;

	//��ȡǰ������
	XMFLOAT3 GetAxisForward() const;
	XMVECTOR GetAxisForwardXM() const;

	//��ȡ����任����
	XMFLOAT4X4 GetLocalToWorldMatrix() const;
	XMMATRIX GetLocalToWorldMatrixXM() const;

	//��ȡ������任����
	XMFLOAT4X4 GetWorldToLocalMatrix() const;
	XMMATRIX GetWorldToLocalMatrixXM() const;


	//���ö������ű���
	void SetScale(const XMFLOAT3& scale);
	void SetScale(float x,float y,float z);

	//���ö����ŷ���ǣ������ƣ���������Z-X-Y��˳����ת
	void SetRotationEuler(const XMFLOAT3& eulerAnglesInRadian);
	void SetRotationEuler(float roll, float pitch, float yaw);

	//���ö���λ��
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//��ԭ����ת������ָ����תŷ����(����ŷ����)
	void Rotate(const XMFLOAT3& eulerAnglesInRadian);
	//��ԭ��Ϊ����������ת������ŷ���ǣ�
	void RotateAxis(const XMFLOAT3& axis, float radian);
	///��ָ����pointΪ����������ת
	void RotatePointAxis(const XMFLOAT3& point, const XMFLOAT3& axis, float radian);

	//��ĳ������ƽ��
	void Translate(const XMFLOAT3& direction, float length);

	//�۲�ĳһ��,���������������ϵת�����������ϵ��ת��ŷ����
	void LookAt(const XMFLOAT3& target, const XMFLOAT3& up = { 0.0f, 1.0f , 0.0f });

	//��ĳ������۲�
	void LookTo(const XMFLOAT3& direction, const XMFLOAT3& up = { 0.0f, 1.0f , 0.0f });

private:
	//����ת�����ȡ��תŷ���Ǹ�ֵ��m_Rotation
	XMFLOAT3 GetEulerAnglesFromRotationMatrix(const XMFLOAT4X4& rotationMatrix);

private:
	XMFLOAT3 m_Scale = { 1.0f,1.0f,1.0f }; //����
	XMFLOAT3 m_Rotation = {}; //��������ԭ����תŷ����(����x,y,z��ֵ)
	XMFLOAT3 m_Position = {}; //����������λ��


};


#endif
