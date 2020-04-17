//
// Created by Alex on 4/4/2020.
//
#include "SystemsScene.h"
#include "game.h"

SystemsScene::SystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer) : BaseScene(gameRef)
{
	double offset = AllmanSquare::_posLimit + _squarePadding;
	int xCount = static_cast<int>(ASGE::SETTINGS.window_width / offset);
	int yCount = static_cast<int>(ASGE::SETTINGS.window_height / offset);

	_memoryManager = std::make_unique<MemoryManager>(sizeof(AllmanSquare), (xCount + 10) * (yCount + 10));
	_jobSystem = std::make_unique<JobSystem>(JobSystem::JobCpuIntensity::HIGH);

	for(int x = -5; x < xCount + 5; ++x)
	{
		for(int y = -5; y < yCount + 5; ++y)
		{
			_squares.emplace_back(renderer, _memoryManager.get(), Vector(static_cast<float>(x * offset), static_cast<float>(y * offset)));
		}
	}
}

SystemsScene::~SystemsScene()
{

}

void SystemsScene::PreUpdate(double dt)
{

}

void SystemsScene::Update(double dt)
{
	//Get the total thread count and half it to get how many threads to use per test
	int threadDivide = static_cast<int>(floor(_jobSystem->GetTotalThreads() / 2));
	int stride = static_cast<int>(floor(static_cast<double>(_squares.size()) / static_cast<double >(threadDivide)));

	//Set up the priority 1 jobs, AKA moving and scaling
	//Set up the priority 2 jobs, AKA position and scale limit check
	for(int i = 0; i < threadDivide; ++i)
	{
		//Start position is the stride distance based of how many of the items we can process for the given threads
		//EndVal progresses just before the next startVal
		int startVal = stride * i;
		int endVal = i == threadDivide - 1 ? static_cast<int>(_squares.size()) - 1 : (stride * (i + 1)) - 1;

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
	for(int j = 0; j < _jobSystem->GetTotalThreads(); ++j)
	{
		int startVal = stride * j;
		int endVal = j == threadDivide - 1 ? static_cast<int>(_squares.size()) - 1 : (stride * (j + 1)) - 1;

		_jobSystem->AddJobToQueue(
				JobSystem::Job(
						std::function<void()>(
								std::bind(&SystemsScene::UpdateSquareColour, this, startVal, endVal, _currentTotalTime)
						)
				)
		);
	}
}

void SystemsScene::PostUpdate(double dt)
{
	_jobSystem->ProcessJobs();
}

void SystemsScene::Render(ASGE::Renderer *renderer)
{
    std::string text = "SYSTEMS SCENE ACTIVE";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.5),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.5),
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
