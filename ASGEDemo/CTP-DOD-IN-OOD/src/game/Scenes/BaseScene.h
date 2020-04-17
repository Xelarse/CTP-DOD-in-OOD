//
// Created by Alex on 4/4/2020.
//
#pragma once
#include "Engine/OGLGame.h"

class MyASGEGame;

class BaseScene
{
public:
    BaseScene() = delete;
    explicit BaseScene(MyASGEGame* gameRef) : _gameRef(gameRef) {};
    virtual ~BaseScene() = default;

    virtual void PreUpdate(double dt) = 0;
    virtual void Update(double dt) = 0;
    virtual void PostUpdate(double dt) = 0;
    virtual void Render(ASGE::Renderer* renderer) = 0;
    virtual void KeyHandler(const ASGE::SharedEventData& data) = 0;
    void UpdateTotalTime(const double& newTime) { _currentTotalTime = newTime; }

protected:
    MyASGEGame* _gameRef = nullptr;
    double _currentTotalTime = 0;
	double _squarePadding = 1.5f;

	int _demoSpanMod = 100;
};