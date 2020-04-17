//
// Created by Alex on 4/14/2020.
//

#include "AllmanSquare.h"

AllmanSquare::AllmanSquare(ASGE::Renderer *renderer, MemoryManager* memoryManager) :
        _basePosition(AllmanVariable<Vector>(memoryManager, AllmanHash("BASEPOSITION"))),
        _position(AllmanVariable<Vector>(memoryManager, AllmanHash("POSITION"))),
        _posReverse(AllmanVariable<bool>(memoryManager, AllmanHash("POSITIONREVERSE"), false)),
        _baseScale(AllmanVariable<float>(memoryManager, AllmanHash("BASESCALE"), 1.0f)),
        _scale(AllmanVariable<float>(memoryManager, AllmanHash("SCALE"), 1.0f)),
        _scaleReverse(AllmanVariable<bool>(memoryManager, AllmanHash("SCALEREVERSE"), false)),
        _squareCol(AllmanVariable<ASGE::Colour>(memoryManager, AllmanHash("SQUARECOLOUR"), ASGE::COLOURS::AZURE))
{
//    InitSprite(renderer);
}

AllmanSquare::AllmanSquare(ASGE::Renderer* renderer, MemoryManager* memoryManager, const Vector& pos) :
    _basePosition(AllmanVariable<Vector>(memoryManager, AllmanHash("BASEPOSITION"), pos)),
    _position(AllmanVariable<Vector>(memoryManager, AllmanHash("POSITION"), pos)),
    _posReverse(AllmanVariable<bool>(memoryManager, AllmanHash("POSITIONREVERSE"), false)),
    _baseScale(AllmanVariable<float>(memoryManager, AllmanHash("BASESCALE"), 1.0f)),
    _scale(AllmanVariable<float>(memoryManager, AllmanHash("SCALE"), 1.0f)),
    _scaleReverse(AllmanVariable<bool>(memoryManager, AllmanHash("SCALEREVERSE"), false)),
    _squareCol(AllmanVariable<ASGE::Colour>(memoryManager, AllmanHash("SQUARECOLOUR"), ASGE::COLOURS::AZURE))
{
//    InitSprite(renderer);
}

void AllmanSquare::InitSprite(ASGE::Renderer* renderer)
{
    _sprite = renderer->createUniqueSprite();
    _sprite->loadTexture("/data/g.png");    //TODO Fix Sprite here
    _sprite->xPos(_position.Get()._x);
    _sprite->yPos(_position.Get()._y);
    _sprite->scale(_scale.Get());
    _sprite->colour(_squareCol.Get());
}

void AllmanSquare::Render(ASGE::Renderer *renderer)
{
//    renderer->renderSprite(*_sprite);
    const int xPos = static_cast<int>(_position.Get()._x);
    const int yPos = static_cast<int>(_position.Get()._y);

    renderer->renderText(
            "&",
            xPos,
            yPos,
            _squareCol.Get()
    );
}

void AllmanSquare::SetColour(ASGE::Colour col)
{
    _squareCol.Set(col);
}

Vector AllmanSquare::GetPosition() const
{
	return _position.Get();
}

void AllmanSquare::SetPosition(const Vector &newPos)
{
	_position.Set(newPos);
}
