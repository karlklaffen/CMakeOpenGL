#include "Timer.h"

Timer::Timer(float max) : MaxTime(max), CurTime(max) {

}

bool Timer::Run(bool reset, float time) {
	CurTime -= time;

	if (CurTime <= 0) {

		if (reset)
			Reset();

		return true;
	}

	return false;
}

bool Timer::Reset() {
	CurTime = MaxTime;
	return false;
}

bool Timer::Reset(float time) {
	MaxTime = time;
	return Reset();
}