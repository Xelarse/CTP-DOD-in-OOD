//
// Created by Alex on 4/6/2020.
//

#include <Engine/Sprite.h>
#include <Engine/Renderer.h>

struct Vector
{
    Vector() = default;
    Vector(float x, float y) : _x(x), _y(y) {};

    float _x = 0;
    float _y = 0;
};

class GameObject
{
public:
    GameObject() = default;
    virtual ~GameObject() = default;

    virtual Vector GetPosition() const = 0;
    virtual void SetPosition(const Vector& newPos) = 0;

    virtual void InitSprite(ASGE::Renderer* renderer) = 0;
    virtual void Update(double dt) = 0;
    virtual void Render(ASGE::Renderer* renderer) = 0;

protected:
    std::unique_ptr<ASGE::Sprite> _sprite;
};


