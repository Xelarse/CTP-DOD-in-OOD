//
// Created by Alex on 4/4/2020.
//
#pragma once
#include "BaseScene.h"
#include "GameObjects/Square.h"

class NoSystemsScene : public BaseScene
{
public:
    NoSystemsScene() = delete;
    explicit NoSystemsScene(MyASGEGame* gameRef, ASGE::Renderer* renderer);
    ~NoSystemsScene() override;

    void PreUpdate(double dt) override;
    void Update(double dt) override;
    void PostUpdate(double dt) override;
    void Render(ASGE::Renderer *renderer) override;

    void KeyHandler(const ASGE::SharedEventData &data) override;

private:
    double _squarePadding = 2.5f;
    std::vector<std::unique_ptr<Square>> _squares;
};
