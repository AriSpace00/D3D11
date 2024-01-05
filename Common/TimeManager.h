#pragma once
class TimeManager
{
public:
	TimeManager();
	~TimeManager();

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_isStop;

public:
	float GetTotalTime() const;
	float GetDeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();
};

