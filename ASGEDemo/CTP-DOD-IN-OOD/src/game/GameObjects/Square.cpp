//
// Created by Alex on 4/6/2020.
//

#include "Square.h"

Square::Square(ASGE::Renderer *renderer)
{
//    InitSprite(renderer);
}

Square::Square(ASGE::Renderer* renderer, const Vector& pos) : _basePosition(pos), _position(pos)
{
//    InitSprite(renderer);
}

Vector Square::GetPosition() const
{
    return _position;
}

void Square::SetPosition(const Vector& newPos)
{
    _position = newPos;
}

void Square::InitSprite(ASGE::Renderer* renderer)
{
    _sprite = renderer->createUniqueSprite();
    _sprite->loadTexture("/data/g.png");    //TODO Fix Sprite here
    _sprite->xPos(_position._x);
    _sprite->yPos(_position._y);
    _sprite->scale(1.0f);
    _sprite->colour(_squareCol);
}

void Square::UpdateSpritePosition(double dt)
{
    _position._x += static_cast<float>(_posReverse ? -dt : dt) * _posMod;

    if(_position._x < _basePosition._x - _posLimit)
    {
        _position._x = static_cast<float>(_basePosition._x - _posLimit);
        _posReverse = !_posReverse;
    }
    else if(_position._x > _basePosition._x + _posLimit)
    {
        _position._x = static_cast<float>(_basePosition._x + _posLimit);
        _posReverse = !_posReverse;
    }
}

void Square::UpdateSpriteScale(double dt)
{
    _scale += static_cast<float>(_scaleReverse ? -dt : dt) * _scaleMod;

    if(_scale < _baseScale - _scaleLimit)
    {
        _scale = static_cast<float>(_baseScale - _scaleLimit);
        _scaleReverse = !_scaleReverse;
    }
    else if(_scale > _baseScale + _scaleLimit)
    {
        _scale = static_cast<float>(_baseScale + _scaleLimit);
        _scaleReverse = !_scaleReverse;
    }
}

void Square::UpdateSpriteColour(double totalTime)
{
    int colourIndex = static_cast<int>(totalTime) % static_cast<int>(_colourChoices.size());

    _squareCol = _colourChoices.at(static_cast<unsigned long long int>(colourIndex));
}

void Square::Render(ASGE::Renderer *renderer)
{
   renderer->renderSprite(*_sprite);
    int xPos = static_cast<int>(_position._x);
    int yPos = static_cast<int>(_position._y);

//    renderer->renderText(
//            "&",
//            xPos,
//            yPos,
//            _squareCol
//        );
}
