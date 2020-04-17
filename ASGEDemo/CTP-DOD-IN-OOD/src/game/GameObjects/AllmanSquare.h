//
// Created by Alex on 4/14/2020.
//
#pragma once
#include "GameObject.h"
#include "../AllmanLibs/MemorySystemsInterface.hpp"

class AllmanSquare : public GameObject
{
public:
    AllmanSquare() = delete;
    explicit AllmanSquare(ASGE::Renderer* renderer, MemoryManager* memoryManager);
    explicit AllmanSquare(ASGE::Renderer* renderer, MemoryManager* memoryManager, const Vector& pos);
    ~AllmanSquare() override = default;

    void InitSprite(ASGE::Renderer* renderer) override;
    void Render(ASGE::Renderer *renderer) override;
    void SetColour(ASGE::Colour col);

    static constexpr double _posLimit = 30.0f;
    static constexpr double _scaleLimit = 0.5f;

    AllmanVariable<Vector>& AllmanPosition() { return _position; }
    AllmanVariable<Vector>& AllmanBasePosition() { return _basePosition; }
    AllmanVariable<bool>& AllmanPositionReverse() { return _posReverse; }

    AllmanVariable<float>& AllmanScale() { return _scale; }
    AllmanVariable<float>& AllmanBaseScale() { return _baseScale; }
    AllmanVariable<bool>& AllmanScaleReverse() { return _scaleReverse; }

private:
    AllmanVariable<Vector> _basePosition;
    AllmanVariable<Vector> _position;
    AllmanVariable<bool> _posReverse;

    AllmanVariable<float> _baseScale;
    AllmanVariable<float> _scale;
    AllmanVariable<bool> _scaleReverse;

    AllmanVariable<ASGE::Colour> _squareCol;
};
