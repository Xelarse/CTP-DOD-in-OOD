#include "..\includes\BaseTest.h"
#include <cmath>

void BaseTest::AddTimeToQueue(long long duration)
{
	//Add the values to the queue, if the queue is already at its limit then pop off the front first

	if (_results.size() >= _queueLength)
	{
		while (_results.size() > _queueLength)
		{
			_results.pop();
		}
	}

	_results.push(duration);
}

void BaseTest::ShieldAdjustment(float* shieldVal)
{
	float modifier = 1.12f;
	float value = *shieldVal;
	int flooredVal = floor(value);

	if (flooredVal % 2 == 0)
	{
		*shieldVal += value * modifier;
	}
	else
	{
		*shieldVal -= value * modifier;
	}
}

void BaseTest::HealthAdjustment(float* healthVal)
{
	*healthVal += 1.25f;

	if (*healthVal > 100)
	{
		*healthVal = 100;
	}
}

void BaseTest::ArmourAdjustment(int* armourVal)
{
	*armourVal -= 1;

	if (*armourVal < 0)
	{
		*armourVal = 20;
	}
}
