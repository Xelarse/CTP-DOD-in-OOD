//
// Created by Alex on 4/6/2020.
//

#include "GameObjects/GameObject.h"

class Square : public GameObject
{
public:
    Square() = delete;
    explicit Square(ASGE::Renderer* renderer);
    explicit Square(ASGE::Renderer* renderer, const Vector& pos);
    ~Square() override = default;

    Vector GetPosition() const override;
    void SetPosition(const Vector &newPos) override;

    void InitSprite(ASGE::Renderer* renderer) override;
    void Update(double dt) override;
    void Render(ASGE::Renderer *renderer) override;

    void ChangeColour(const ASGE::Colour& colour);

private:
    Vector _position;
    ASGE::Colour _squareCol = ASGE::COLOURS::AZURE;
};

