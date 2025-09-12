#pragma once
class Timer {

public:
	Timer(float max);

	bool Run(bool reset = true, float time = 1);

	bool Reset();
	bool Reset(float time);

private:
	float MaxTime, CurTime;
};