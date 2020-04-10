//
// Created by Alex on 4/6/2020.
//

#include "Square.h"

Square::Square(ASGE::Renderer *renderer)
{
    InitSprite(renderer);
}

Square::Square(ASGE::Renderer* renderer, const Vector& pos) : _position(pos)
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
    _sprite->scale(1.0f);
    _sprite->colour(_squareCol);
}

void Square::Update(double dt)
{
    _sprite->xPos(_position._x);
    _sprite->yPos(_position._y);
    _sprite->colour(_squareCol);
}

void Square::Render(ASGE::Renderer *renderer)
{
    renderer->renderSprite(*_sprite);
}

void Square::ChangeColour(const ASGE::Colour &colour)
{
    _squareCol = colour;
}
