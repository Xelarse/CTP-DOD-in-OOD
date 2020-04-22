//
// Created by Alex on 4/6/2020.
//
#pragma once
#include "BaseScene.h"

class MenuScene : public BaseScene
{
public:
    MenuScene() = delete;
    explicit MenuScene(MyASGEGame* gameRef);
    ~MenuScene() override = default;

    void PreUpdate(double dt) override;
    void Update(double dt) override;
    void PostUpdate(double dt) override;
    void Render(ASGE::Renderer *renderer) override;
    void KeyHandler(const ASGE::SharedEventData &data) override;


private:
    enum class Selection
    {
        NO_SYSTEMS = 0,
        SYSTEMS = 1,
        JUST_MEM = 2,
        QUIT = 3,
        COUNT = 4
    };

    Selection GetCurrentSelection();

    int _currentSelection = 0;
};


