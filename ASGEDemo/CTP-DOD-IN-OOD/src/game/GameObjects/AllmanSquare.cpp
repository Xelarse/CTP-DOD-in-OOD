//
// Created by Alex on 4/14/2020.
//

#include "AllmanSquare.h"

AllmanSquare::AllmanSquare(ASGE::Renderer *renderer, MemoryManager* memoryManager) :
        _basePosition(AllmanVariable<Vector>(memoryManager, AllmanHash("BASEPOSITION"))),
        _position(AllmanVariable<Vector>(memoryManager, AllmanHash("POSITION"))),
        _posReverse(AllmanVariable<bool>(memoryManager, AllmanHash("POSITIONREVERSE"), false)),
        _baseScale(AllmanVariable<float>(memoryManager, AllmanHash("BASESCALE"), 0.075f)),
        _scale(AllmanVariable<float>(memoryManager, AllmanHash("SCALE"), 0.075f)),
        _scaleReverse(AllmanVariable<bool>(memoryManager, AllmanHash("SCALEREVERSE"), false)),
        _squareCol(AllmanVariable<ASGE::Colour>(memoryManager, AllmanHash("SQUARECOLOUR"), ASGE::COLOURS::AZURE))
{
}

AllmanSquare::AllmanSquare(ASGE::Renderer* renderer, MemoryManager* memoryManager, const Vector& pos) :
    _basePosition(AllmanVariable<Vector>(memoryManager, AllmanHash("BASEPOSITION"), pos)),
    _position(AllmanVariable<Vector>(memoryManager, AllmanHash("POSITION"), pos)),
    _posReverse(AllmanVariable<bool>(memoryManager, AllmanHash("POSITIONREVERSE"), false)),
    _baseScale(AllmanVariable<float>(memoryManager, AllmanHash("BASESCALE"), 0.075f)),
    _scale(AllmanVariable<float>(memoryManager, AllmanHash("SCALE"), 0.075f)),
    _scaleReverse(AllmanVariable<bool>(memoryManager, AllmanHash("SCALEREVERSE"), false)),
    _squareCol(AllmanVariable<ASGE::Colour>(memoryManager, AllmanHash("SQUARECOLOUR"), ASGE::COLOURS::AZURE))
{
}

void AllmanSquare::InitSprite(ASGE::Renderer* renderer)
{
    _sprite = renderer->createUniqueSprite();
    _sprite->loadTexture("/data/circle.png");
    _sprite->xPos(_position.Get()._x);
    _sprite->yPos(_position.Get()._y);
    _sprite->scale(_scale.Get());
    _sprite->colour(_squareCol.Get());
}

void AllmanSquare::Render(ASGE::Renderer *renderer)
{
    renderer->renderSprite(*(_sprite.get()));
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

void AllmanSquare::UpdateSprite()
{
	_sprite->xPos(_position.Get()._x);
	_sprite->scale(_scale.Get());
	_sprite->colour(_squareCol.Get());
}
