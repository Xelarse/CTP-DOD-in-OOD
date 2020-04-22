//
// Created by Alex on 4/4/2020.
//
#include "SystemsScene.h"
#include "game.h"

#include <Engine/Logger.hpp>

SystemsScene::SystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer, int demoSpan) : BaseScene(gameRef), _demoSpanMod(demoSpan)
{
      double offset = AllmanSquare::_posLimit + _squarePadding;
      int xCount = static_cast<int>(ASGE::SETTINGS.window_width / offset);
      int yCount = static_cast<int>(ASGE::SETTINGS.window_height / offset);

      _memoryManager = std::make_unique<MemoryManager>(sizeof(AllmanSquare), (xCount + 2 *_demoSpanMod) * (yCount + 2 *_demoSpanMod) + 1);
      _jobSystem = std::make_unique<JobSystem>(JobSystem::JobCpuIntensity::MEDIUM);
      _squares.reserve(static_cast<unsigned long long int>((xCount + 2 * _demoSpanMod) * (yCount + 2 * _demoSpanMod)));

      for(int x = -_demoSpanMod; x < xCount + _demoSpanMod; ++x)
      {
        for(int y = -_demoSpanMod; y < yCount + _demoSpanMod; ++y)
        {
          _squares
              .emplace_back(renderer, _memoryManager.get(),
                            Vector(static_cast<float>(x * offset),
                                   static_cast<float>(y * offset)))
              .InitSprite(renderer);
        }
      }

      //Setup thread and stride counts used in the update loop based on job system thread count
      double threadsForTasks = floor(_jobSystem->GetTotalThreads() / 2);
      _jobPriorityThreadCounts[0] = static_cast<int>(threadsForTasks);
      _jobPriorityThreadCounts[1] = static_cast<int>(threadsForTasks) - 1;
      _jobPriorityStrideCounts[0] = static_cast<int>(floor(static_cast<double>(_squares.size()) / static_cast<double>(_jobPriorityThreadCounts[0])));
      _jobPriorityStrideCounts[1] = static_cast<int>(floor(static_cast<double>(_squares.size()) / static_cast<double>(_jobPriorityThreadCounts[1])));
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
	else
	{
		for(auto& square : _squares)
		{
			square.UpdateSprite();
		}
	}
}

void SystemsScene::Render(ASGE::Renderer *renderer)
{
	for(auto& square : _squares)
	{
		//Sorry in advance for this james, Checking in they're in window view for rendering
		//Yeah might change this to use camera bounds if i mess with the camera view
		Vector pos = square.AllmanPosition().Get();
                if (pos._x >= -50 &&
                    pos._x <= static_cast<float>(ASGE::SETTINGS.window_width) + 50 &&
                    pos._y >= 50 &&
                    pos._y <= static_cast<float>(ASGE::SETTINGS.window_height) + 50
                    )
                {
                  square.Render(renderer);
                }
        }

        renderer->renderText(
            "Entites updated per tick: " + std::to_string(_squares.size()),
            1120,
            30,
            ASGE::COLOURS::BLACK
        );
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
	//Set up the priority 1 jobs, AKA moving and scaling
	for(int j = 0; j < _jobPriorityThreadCounts[0]; ++j)
	{
		//Start position is the stride distance based of how many of the items we can process for the given threads
		//EndVal progresses just before the next startVal
		int startVal = _jobPriorityStrideCounts[0] * j;
		int endVal = j == _jobPriorityThreadCounts[0] - 1 ? static_cast<int>(_squares.size()) - 1 : (_jobPriorityStrideCounts[0] * (j + 1)) - 1;

		//Job to update square position and scale with first priority
		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								[this, startVal, endVal, dt] { UpdateSquarePosition(startVal, endVal, dt); }
						),
						JobSystem::Job::JobPriority::ORDERED,
						1
				)
		);

		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								[this, startVal, endVal, dt] { UpdateSquareScale(startVal, endVal, dt); }
						),
						JobSystem::Job::JobPriority::ORDERED,
						1
				)
		);
	}

	//Set up the priority 2 jobs, AKA position and scale limit check
	for(int i = 0; i < _jobPriorityThreadCounts[1]; ++i)
	{
		//Start position is the stride distance based of how many of the items we can process for the given threads
		//EndVal progresses just before the next startVal
		int startVal = _jobPriorityStrideCounts[1] * i;
		int endVal = i == _jobPriorityThreadCounts[1] - 1
                                 ? static_cast<int>(_squares.size()) - 1
                                 : (_jobPriorityStrideCounts[1] * (i + 1)) - 1;

                //Job to check square position and scale bounds with second priority
		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								[this, startVal, endVal] { SquarePositionBoundCheck(startVal, endVal); }
						),
						JobSystem::Job::JobPriority::ORDERED,
						2
				)
		);

		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								[this, startVal, endVal] { SquareScaleBoundCheck(startVal, endVal); }
						),
						JobSystem::Job::JobPriority::ORDERED,
						2
				)
		);
	}

	const int totalThreads = _jobSystem->GetTotalThreads();
	const int stride = static_cast<int>(floor(static_cast<double>(_squares.size() /
	                                                              static_cast<unsigned long long int>(totalThreads))));

	for(int i = 0; i < totalThreads; ++i)
	{
		int startInd = stride * i;
		int endInd = i == totalThreads - 1 ? static_cast<int>(_squares.size()) - 1 : stride * (i + 1) - 1;

		_jobSystem->AddJobToQueue(JobSystem::Job(
					std::function<void()>(
							[this, startInd, endInd] { UpdateSquareColour(startInd, endInd, _currentTotalTime); }
					),
					JobSystem::Job::JobPriority::ORDERED,
					3
				)
		);

		_jobSystem->AddJobToQueue(JobSystem::Job(
				std::function<void()>(
						[this, startInd, endInd] { SetSquareSprites(startInd, endInd); }
				),
				JobSystem::Job::JobPriority::ORDERED,
				4
              )
		);
	}
}

void SystemsScene::UpdateSquarePosition(int startInd, int endInd, double dt)
{
    Vector* baseVectorPtr = _squares.front().AllmanPosition().GetBasePtr();
    bool* baseBoolPtr = _squares.front().AllmanPositionReverse().GetBasePtr();

    for(int i = startInd; i <= endInd; ++i)
    {
        Vector* currentPosPtr = baseVectorPtr + i;
        const bool* currentBoolPtr = baseBoolPtr + i;
        currentPosPtr->_x += static_cast<float>(*currentBoolPtr ? -dt : dt) * 0.05f;
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
        *currentScalePtr += static_cast<float>(*currentBoolPtr ? -dt : dt) * 0.00005f;
    }
}

void SystemsScene::UpdateSquareColour(int startInd, int endInd, double totalTime)
{
    const int colourIndex = static_cast<int>(totalTime * 0.001) % static_cast<int>(_colourChoices.size());
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

void SystemsScene::SetSquareSprites(int startInd, int endInd)
{
	for(int i = startInd; i <= endInd; ++i)
	{
		_squares[static_cast<unsigned long long int>(i)].UpdateSprite();
	}
}
