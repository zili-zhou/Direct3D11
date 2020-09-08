#pragma once
#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;   //����Ϸʱ��
	float DeltaTime() const;   //֡���ʱ��

	void Reset();              //��Ϣѭ��ǰ����
	void Start();              //ȡ����ͣ
	void Stop();               //��ʼ��ͣ
	void Tick();               //ÿһ֡ʱ����

private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_Stopped;
};



#endif // GAMETIMER_H
