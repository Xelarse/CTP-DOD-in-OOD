//
// Created by Alex on 4/14/2020.
//

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

private:
    AllmanVariable<Vector> _basePosition;
    AllmanVariable<Vector> _position;
    AllmanVariable<bool> _posReverse;

    AllmanVariable<float> _baseScale;
    AllmanVariable<float> _scale;
    AllmanVariable<bool> _scaleReverse;

    ASGE::Colour _squareCol = ASGE::COLOURS::AZURE;
};
