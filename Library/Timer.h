#pragma once

#include <Windows.h>

class Timer
{
private:
	Timer()
	{
		LONGLONG counts_per_sec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		secondsPerCount = 1.0 / static_cast<double>(counts_per_sec);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;
	}
	~Timer() = default;
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer(Timer&&) noexcept = delete;
	Timer& operator=(Timer&&) noexcept = delete;

public:
	static Timer& Instance()
	{
		static Timer instance;
		return instance;
	}

private:
	double secondsPerCount{ 0.0 };
	double deltaTime{ 0.0 };
	double storeDeltaTime{ 0.0 };
	double checkTime{ 0.0 };

	double updateTime = 0.0f;
	double storeUpdateTime = 0.0f;
	double renderTime = 0.0f;
	double storeRenderTime = 0.0f;

	LONGLONG baseTime{ 0LL };
	LONGLONG pausedTime{ 0LL };
	LONGLONG stopTime{ 0LL };
	LONGLONG lastTime{ 0LL };
	LONGLONG thisTime{ 0LL };

	bool stopped{ false };

	float timeScale = 1.0f;

public:

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is stopped.
	float TimeStamp() const  // in seconds
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance
		// stopTime - baseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:
		//
		//                     |<--pausedTime-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  baseTime       stopTime        start_time     stopTime    thisTime

		if (stopped)
		{
			return static_cast<float>(((stopTime - pausedTime) - baseTime) * secondsPerCount);
		}

		// The distance thisTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract
		// the paused time from thisTime:
		//
		//  (thisTime - pausedTime) - baseTime
		//
		//                     |<--pausedTime-->|
		// ----*---------------*-----------------*------------*------> time
		//  baseTime       stopTime        start_time     thisTime
		else
		{
			return static_cast<float>(((thisTime - pausedTime) - baseTime) * secondsPerCount);
		}
	}

	float TimeInterval() const  // in seconds
	{
		return static_cast<float>(deltaTime);
	}

	void Reset() // Call before message loop.
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;

		stopTime = 0;
		stopped = false;
	}

	void Start() // Call when unpaused.
	{
		LONGLONG start_time;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  baseTime       stopTime        start_time
		if (stopped)
		{
			pausedTime += (start_time - stopTime);
			lastTime = start_time;
			stopTime = 0;
			stopped = false;
		}
	}

	void Stop() // Call when paused.
	{
		if (!stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stopTime));
			stopped = true;
		}
	}

	void Tick() // Call every frame.
	{
		if (stopped)
		{
			deltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		// Time difference between this frame and the previous.
		storeDeltaTime = deltaTime;
		deltaTime = (thisTime - lastTime) * secondsPerCount;

		// Prepare for next frame.
		lastTime = thisTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (deltaTime < 0.0)
		{
			deltaTime = 0.0;
		}
	}

	// --- �Z�b�^�[ ---
	void SetDeltaTime(float t)
	{
		deltaTime = static_cast<double>(t);
	}

	void SetTimeScale(const float t) { timeScale = t; }

	// --- �Q�b�^�[ ---
	float DeltaTime() const
	{
		return static_cast<float>(deltaTime) * timeScale;
	}
	float StoreDeltaTime() const
	{
		return static_cast<float>(storeDeltaTime);
	}
	float UnscaledDeltaTime() const
	{
		return static_cast<float>(deltaTime);
	}

	void CheckTick()
	{
		if (stopped)
		{
			checkTime = 0.0f;
			return;
		}


		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		// Time difference between this frame and the previous.
		checkTime = (thisTime - lastTime) * secondsPerCount;

		// Prepare for next frame.
		lastTime = thisTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (checkTime < 0.0)
		{
			checkTime = 0.0;
		}
	}

	float CheckTime() const
	{
		return static_cast<float>(checkTime);
	}

	// ----- UpdateTime ------
	void StartUpdateTime()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		updateTime = thisTime;
	}
	void StopUpdateTime()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		updateTime = (thisTime - updateTime) * secondsPerCount;
		storeUpdateTime = updateTime;
	}
	float UpdateTime() const
	{
		return static_cast<float>(updateTime);
	}
	float StoreUpdateTime() const
	{
		return static_cast<float>(storeUpdateTime);
	}

	// ----- RenderTime -----
	void StartRenderTime()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		renderTime = thisTime;
	}
	void StopRenderTime()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		renderTime = (thisTime - renderTime) * secondsPerCount;
		storeRenderTime = renderTime;
	}
	float RenderTime() const
	{
		return static_cast<float>(renderTime);
	}
	float StoreRenderTime() const
	{
		return static_cast<float>(storeRenderTime);
	}

	//float UpdateTime() { return updateTime; }
};