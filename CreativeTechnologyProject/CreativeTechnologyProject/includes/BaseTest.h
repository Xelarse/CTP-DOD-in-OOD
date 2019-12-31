#pragma once

#include <queue>

class BaseTest
{
public:

	BaseTest() = default;
	virtual ~BaseTest() = default;

	virtual void PreUpdate(float dt) = 0;
	virtual void Update(float dt) = 0;
	virtual void PostUpdate(float dt) = 0;

	int RunCount() const { return _runCount; }
	void RunCount(int x) { _runCount = x; }

	void AddTimeToQueue(long long duration);
	virtual void RenderImguiWindow() = 0;

protected:
	void ShieldAdjustment(float* shieldVal);
	void HealthAdjustment(float* healthVal);
	void ArmourAdjustment(int* armourVal);

	int _runCount = 1;

	const int _queueLength = 200;
	std::deque<long long> _results;
	
};

