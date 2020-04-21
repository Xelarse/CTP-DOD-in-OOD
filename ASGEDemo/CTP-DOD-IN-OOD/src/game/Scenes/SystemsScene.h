//
// Created by Alex on 4/4/2020.
//
#pragma once
#include "BaseScene.h"
#include "GameObjects/AllmanSquare.h"
#include "../AllmanLibs/MemorySystems.hpp"
#include "../AllmanLibs/JobSystem.hpp"

class SystemsScene : public BaseScene
{
public:
    SystemsScene() = delete;
    explicit SystemsScene(MyASGEGame* gameRef, ASGE::Renderer* renderer, int demoSpan);
    ~SystemsScene() override = default;

    void PreUpdate(double dt) override;
    void Update(double dt) override;
    void PostUpdate(double dt) override;
    void Render(ASGE::Renderer *renderer) override;
    void KeyHandler(const ASGE::SharedEventData &data) override;

private:
    void SetJobsForUpdate(double dt);
    void UpdateSquarePosition(int startInd, int endInd, double dt);
    void UpdateSquareScale(int startInd, int endInd, double dt);
    void UpdateSquareColour(int startInd, int endInd, double totalTime);
    void SquarePositionBoundCheck(int startInd, int endInd);
    void SquareScaleBoundCheck(int startInd, int endInd);
    void SetSquareSprites(int startInd, int endInd);


    bool _jobSystemActive = true;
    std::unique_ptr<JobSystem> _jobSystem;
    std::unique_ptr<MemoryManager> _memoryManager;
    std::vector<AllmanSquare> _squares;

    std::array<int, 2> _jobPriorityThreadCounts;
    std::array<int, 2> _jobPriorityStrideCounts;

    std::array<ASGE::Colour, 7> _colourChoices = {
            ASGE::Colour(1.0f, 0.058f, 0.039f),         //R
            ASGE::Colour(1.0f, 0.729f, 0.039f),         //O
            ASGE::Colour(0.976f, 1.0f, 0.039f),         //Y
            ASGE::Colour(0.039f, 1.0f, 0.129f),         //G
            ASGE::Colour(0.039f, 0.294f, 1.0f),         //B
            ASGE::Colour(0.552f, 0.039f, 1.0f),         //I
            ASGE::Colour(0.972f, 0.039f, 1.0f)          //V
    };

    const int _demoSpanMod;
};
