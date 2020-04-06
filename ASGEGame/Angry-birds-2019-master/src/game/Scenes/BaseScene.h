//
// Created by Alex on 4/4/2020.
//
#pragma once
#include "Engine/OGLGame.h"

class BaseScene
{
public:
    BaseScene() = default;
    virtual ~BaseScene() = default;

    virtual void PreUpdate(float dt) = 0;
    virtual void Update(float dt) = 0;
    virtual void PostUpdate(float dt) = 0;
    virtual void Render(ASGE::Renderer* renderer) = 0;
    virtual void KeyHandler(const ASGE::SharedEventData& data) = 0;
};