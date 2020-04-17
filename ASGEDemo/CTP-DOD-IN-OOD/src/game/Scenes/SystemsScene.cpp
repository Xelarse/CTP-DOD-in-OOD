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

	//const size_t elementSize, const size_t elementCount
	//JobCpuIntensity intensity, bool quickSetup = true

	_memoryManager = std::make_unique<MemoryManager>(sizeof(AllmanSquare), (xCount + 10) * (yCount + 10));
	_jobSystem = std::make_unique<JobSystem>(JobSystem::JobCpuIntensity::HIGH);

	for(int x = -5; x < xCount + 5; ++x)
	{
		for(int y = -5; y < yCount + 5; ++y)
		{
//			_squares.emplace_back(renderer, _memoryManager.get(), Vector(static_cast<float>(x * offset), static_cast<float>(y * offset)));
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
    // Add job for moving all sprites, pri 1
    // Add job for scaling all sprites, pri 1
    // Add job to colour all sprites, unordered

    //Add job to check boundaries and if over then flip bool
    //This applies to scale and position, pri 2
}

void SystemsScene::PostUpdate(double dt)
{
    //Invoke the jobs
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
