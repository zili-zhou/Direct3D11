#pragma once
#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;   //总游戏时间
	float DeltaTime() const;   //帧间隔时间

	void Reset();              //消息循环前调用
	void Start();              //取消暂停
	void Stop();               //开始暂停
	void Tick();               //每一帧时调用

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
