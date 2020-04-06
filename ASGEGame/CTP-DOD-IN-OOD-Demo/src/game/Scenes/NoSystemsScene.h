//
// Created by Alex on 4/4/2020.
//
#pragma once
#include "BaseScene.h"

class NoSystemsScene : public BaseScene
{
public:
    NoSystemsScene() = delete;
    explicit NoSystemsScene(MyASGEGame* gameRef);
    ~NoSystemsScene() override;

    void PreUpdate(double dt) override;
    void Update(double dt) override;
    void PostUpdate(double dt) override;
    void Render(ASGE::Renderer *renderer) override;

    void KeyHandler(const ASGE::SharedEventData &data) override;
};
