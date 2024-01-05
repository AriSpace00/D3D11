#include "pch.h"
#include "TimeManager.h"

TimeManager::TimeManager()
	: m_secondsPerCount(0.0)
	, m_deltaTime(-1.0)
	, m_baseTime(0)
	, m_pausedTime(0)
	, m_stopTime(0)
	, m_prevTime(0)
	, m_currTime(0)
	, m_isStop(false)
{

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

TimeManager::~TimeManager()
{
}

float TimeManager::GetTotalTime() const
{
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       m_stopTime        startTime     m_stopTime    m_currTime

	if (m_isStop)
	{
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}

}

float TimeManager::GetDeltaTime() const
{
	return (float)m_deltaTime;
}

void TimeManager::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_isStop = false;
}

void TimeManager::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       m_stopTime        startTime     

	if (m_isStop)
	{
		m_pausedTime += (startTime - m_stopTime);

		m_prevTime = startTime;
		m_stopTime = 0;
		m_isStop = false;
	}
}

void TimeManager::Stop()
{
	if (!m_isStop)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_stopTime = currTime;
		m_isStop = true;
	}
}

void TimeManager::Tick()
{
	if (m_isStop)
	{
		m_deltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// Time difference between this frame and the previous.
	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

	// Prepare for next frame.
	m_prevTime = m_currTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then m_deltaTime can be negative.
	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}
