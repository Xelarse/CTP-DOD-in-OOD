//
// Created by Alex on 4/6/2020.
//

#include "Square.h"

#include <Engine/Logger.hpp>

Square::Square(ASGE::Renderer *renderer)
{
    InitSprite(renderer);
}

Square::Square(ASGE::Renderer* renderer, const Vector& pos) : _basePosition(pos), _position(pos)
{
    InitSprite(renderer);
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
    _sprite->scale(_scale);
    _sprite->colour(_squareCol);
}

void Square::UpdateSpritePosition(double dt)
{
    _position._x += static_cast<float>(_posReverse ? -dt : dt) * _posMod;

    if(_position._x < _basePosition._x - _posLimit)
    {
        _position._x = _basePosition._x - _posLimit;
        _posReverse = !_posReverse;
    }
    else if(_position._x > _basePosition._x + _posLimit)
    {
        _position._x = _basePosition._x + _posLimit;
        _posReverse = !_posReverse;
    }

    _sprite->xPos(_position._x);
}

void Square::UpdateSpriteScale(double dt)
{
    _scale += static_cast<float>(_scaleReverse ? -dt : dt) * _scaleMod;

    if(_scale < _baseScale - _scaleLimit)
    {
        _scale = _baseScale - _scaleLimit;
        _scaleReverse = !_scaleReverse;
    }
    else if(_scale > _baseScale + _scaleLimit)
    {
        _scale = _baseScale + _scaleLimit;
        _scaleReverse = !_scaleReverse;
    }

    _sprite->scale(_scale);
}

void Square::UpdateSpriteColour(double totalTime)
{
    int colourIndex = static_cast<int>(totalTime * 0.001) % static_cast<int>(_colourChoices.size());
    _squareCol = _colourChoices.at(static_cast<unsigned long long int>(colourIndex));
    _sprite->colour(_squareCol);
}

void Square::Render(ASGE::Renderer *renderer)
{
   renderer->renderSprite(*_sprite);
   Logging::INFO("Square Size: " + std::to_string(_sprite->scale()));
}
