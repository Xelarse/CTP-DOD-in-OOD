//
// Created by Alex on 4/4/2020.
//
#pragma once
#include "BaseScene.h"

class NoSystemsScene : public BaseScene
{
public:
    NoSystemsScene();
    ~NoSystemsScene() override;

    void PreUpdate(float dt) override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Render(ASGE::Renderer *renderer) override;

    void KeyHandler(const ASGE::SharedEventData &data) override;
};
