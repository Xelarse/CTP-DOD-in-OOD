//
// Created by Alex on 4/4/2020.
//
#include "SystemsScene.h"
#include "game.h"
#include <Engine/Logger.hpp>

SystemsScene::SystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer) : BaseScene(gameRef)
{
	double offset = AllmanSquare::_posLimit + _squarePadding;
	int xCount = static_cast<int>(ASGE::SETTINGS.window_width / offset);
	int yCount = static_cast<int>(ASGE::SETTINGS.window_height / offset);

	_memoryManager = std::make_unique<MemoryManager>(sizeof(AllmanSquare), (xCount + 2 *_demoSpanMod) * (yCount + 2 *_demoSpanMod));
	_jobSystem = std::make_unique<JobSystem>(JobSystem::JobCpuIntensity::MEDIUM);

	for(int x = -_demoSpanMod; x < xCount + _demoSpanMod; ++x)
	{
		for(int y = -_demoSpanMod; y < yCount + _demoSpanMod; ++y)
		{
			_squares.emplace_back(renderer, _memoryManager.get(), Vector(static_cast<float>(x * offset), static_cast<float>(y * offset)));
		}
	}

	std::ostringstream ss{ "System scene square count :" + std::to_string(_squares.size()) };
	Logging::INFO(ss.str());
}

SystemsScene::~SystemsScene()
{

}

void SystemsScene::PreUpdate(double dt)
{

}

void SystemsScene::Update(double dt)
{
	//If job system isn't active just process them single threaded using memory locality otherwise setup the jobs for the update
	if(_jobSystemActive)
	{
		SetJobsForUpdate(dt);
	}
	else
	{
		UpdateSquarePosition(0, static_cast<int>(_squares.size() - 1), dt);
		UpdateSquareScale(0, static_cast<int>(_squares.size() - 1), dt);
		SquarePositionBoundCheck(0, static_cast<int>(_squares.size() - 1));
		SquareScaleBoundCheck(0, static_cast<int>(_squares.size() - 1));
		UpdateSquareColour(0, static_cast<int>(_squares.size() - 1), _currentTotalTime);
	}
}

void SystemsScene::PostUpdate(double dt)
{
	//If job system is active processed the jobs set
	if(_jobSystemActive)
	{
		_jobSystem->ProcessJobs();
	}
}

void SystemsScene::Render(ASGE::Renderer *renderer)
{
//	for(auto& square : _squares)
//	{
//		square.Render(renderer);
//	}
}

void SystemsScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_ESCAPE)
    {
        _gameRef->ChangeScene(MyASGEGame::Scenes::MENU);
    }
}

void SystemsScene::SetJobsForUpdate(double dt)
{
	//Splitting the total job count into chunks to assign to various jobs
	double threadsForTasks = floor(_jobSystem->GetTotalThreads() / 2);
	int firstJobs = static_cast<int>(threadsForTasks);
	int secondJobs = static_cast<int>(threadsForTasks) - 1;
	int firstStride = static_cast<int>(floor(static_cast<double>(_squares.size()) / static_cast<double >(firstJobs)));
	int secondStride = static_cast<int>(floor(static_cast<double>(_squares.size()) / static_cast<double >(secondJobs)));

	//Set up the priority 1 jobs, AKA moving and scaling
	for(int j = 0; j < firstJobs; ++j)
	{
		//Start position is the stride distance based of how many of the items we can process for the given threads
		//EndVal progresses just before the next startVal
		int startVal = firstStride * j;
		int endVal = j == firstJobs - 1 ? static_cast<int>(_squares.size()) - 1 : (firstStride * (j + 1)) - 1;

		//Job to update square position and scale with first priority
		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								std::bind(&SystemsScene::UpdateSquarePosition, this, startVal, endVal, dt)
						),
						JobSystem::Job::JobPriority::ORDERED,
						1
				)
		);

		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								std::bind(&SystemsScene::UpdateSquareScale, this, startVal, endVal, dt)
						),
						JobSystem::Job::JobPriority::ORDERED,
						1
				)
		);
	}

	//Set up the priority 2 jobs, AKA position and scale limit check
	for(int i = 0; i < secondJobs; ++i)
	{
		//Start position is the stride distance based of how many of the items we can process for the given threads
		//EndVal progresses just before the next startVal
		int startVal = secondStride * i;
		int endVal = i == secondJobs - 1 ? static_cast<int>(_squares.size()) - 1 : (secondStride * (i + 1)) - 1;

		//Job to check square position and scale bounds with second priority
		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								std::bind(&SystemsScene::SquarePositionBoundCheck, this, startVal, endVal)
						),
						JobSystem::Job::JobPriority::ORDERED,
						2
				)
		);

		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								std::bind(&SystemsScene::SquareScaleBoundCheck, this, startVal, endVal)
						),
						JobSystem::Job::JobPriority::ORDERED,
						2
				)
		);
	}

	//Set up the unordered jobs, AKA Colour switching of all sprites
	_jobSystem->AddJobToQueue(
			JobSystem::Job(
					std::function<void()>(
							std::bind(&SystemsScene::UpdateSquareColour, this, 0, _squares.size() - 1, _currentTotalTime)
					)
			)
	);
}

void SystemsScene::UpdateSquarePosition(int startInd, int endInd, double dt)
{
    Vector* baseVectorPtr = _squares.front().AllmanPosition().GetBasePtr();
    bool* baseBoolPtr = _squares.front().AllmanPositionReverse().GetBasePtr();

    for(int i = startInd; i <= endInd; ++i)
    {
        Vector* currentPosPtr = baseVectorPtr + i;
        const bool* currentBoolPtr = baseBoolPtr + i;
        currentPosPtr->_x += static_cast<float>(*currentBoolPtr ? -dt : dt) * 0.2f;
    }
}

void SystemsScene::UpdateSquareScale(int startInd, int endInd, double dt)
{
    float* baseScalePtr = _squares.front().AllmanScale().GetBasePtr();
    bool* baseBoolPtr = _squares.front().AllmanScaleReverse().GetBasePtr();

    for(int i = startInd; i <= endInd; ++i)
    {
        float* currentScalePtr = baseScalePtr + i;
        const bool* currentBoolPtr = baseBoolPtr + i;
        *currentScalePtr += static_cast<float>(*currentBoolPtr ? -dt : dt) * 0.4f;
    }
}

void SystemsScene::UpdateSquareColour(int startInd, int endInd, double totalTime)
{
    const int colourIndex = static_cast<int>(totalTime) % static_cast<int>(_colourChoices.size());
    const ASGE::Colour outCol = _colourChoices.at(static_cast<unsigned long long int>(colourIndex));

    ASGE::Colour* baseColourPtr = _squares.front().AllmanColour().GetBasePtr();

    for(int i = startInd; i <= endInd; ++i)
    {
        ASGE::Colour* currentColourPtr = baseColourPtr + i;
        *currentColourPtr = outCol;
    }
}

void SystemsScene::SquarePositionBoundCheck(int startInd, int endInd)
{
    Vector* baseBasePositionPtr = _squares.front().AllmanBasePosition().GetBasePtr();
    Vector* basePositionPtr = _squares.front().AllmanPosition().GetBasePtr();
    bool* baseBoolPtr = _squares.front().AllmanPositionReverse().GetBasePtr();

    for(int i = startInd; i <= endInd; ++i)
    {
        Vector* currentBasePositionPtr = baseBasePositionPtr + i;
        Vector* currentPositionPtr = basePositionPtr + i;
        bool* currentBoolPtr = baseBoolPtr + i;

        if(currentPositionPtr->_x < currentBasePositionPtr->_x - AllmanSquare::_posLimit)
        {
            currentPositionPtr->_x = currentBasePositionPtr->_x - AllmanSquare::_posLimit;
            *currentBoolPtr = !*currentBoolPtr;
        }
        else if(currentPositionPtr->_x > currentBasePositionPtr->_x + AllmanSquare::_posLimit)
        {
            currentPositionPtr->_x = currentBasePositionPtr->_x + AllmanSquare::_posLimit;
            *currentBoolPtr = !*currentBoolPtr;
        }
    }
}

void SystemsScene::SquareScaleBoundCheck(int startInd, int endInd)
{
    float* baseBaseScalePtr = _squares.front().AllmanBaseScale().GetBasePtr();
    float* baseScalePtr = _squares.front().AllmanScale().GetBasePtr();
    bool* baseBoolPtr = _squares.front().AllmanScaleReverse().GetBasePtr();

    for(int i = startInd; i <= endInd; ++i)
    {
        float* currentBaseScalePtr = baseBaseScalePtr + i;
        float* currentScalePtr = baseScalePtr + i;
        bool* currentBoolPtr = baseBoolPtr + i;

        if(*currentScalePtr < *currentBaseScalePtr - AllmanSquare::_scaleLimit)
        {
	        *currentScalePtr = *currentBaseScalePtr - AllmanSquare::_scaleLimit;
            *currentBoolPtr = !*currentBoolPtr;
        }
        else if(*currentScalePtr > *currentBaseScalePtr + AllmanSquare::_scaleLimit)
        {
	        *currentScalePtr = *currentBaseScalePtr + AllmanSquare::_scaleLimit;
            *currentBoolPtr = !*currentBoolPtr;
        }
    }
}
