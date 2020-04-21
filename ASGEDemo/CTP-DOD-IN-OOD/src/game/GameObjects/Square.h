//
// Created by Alex on 4/6/2020.
//
#pragma once
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
    void UpdateSpritePosition(double dt);
    void UpdateSpriteScale(double dt);
    void UpdateSpriteColour(double totalTime);


    void Render(ASGE::Renderer *renderer) override;

    static constexpr float _posLimit = 30.0f;

private:
    Vector _basePosition;
    Vector _position;
    float _posMod = 0.05f;
    bool _posReverse = false;

    float _baseScale = 0.075f;
    float _scale = 0.075f;
    float _scaleLimit = 0.01f;
    float _scaleMod = 0.00005f;
    bool _scaleReverse = false;

    ASGE::Colour _squareCol = ASGE::COLOURS::AZURE;

    std::array<ASGE::Colour, 7> _colourChoices = {
            ASGE::Colour(1.0f, 0.058f, 0.039f),         //R
            ASGE::Colour(1.0f, 0.729f, 0.039f),         //O
            ASGE::Colour(0.976f, 1.0f, 0.039f),         //Y
            ASGE::Colour(0.039f, 1.0f, 0.129f),         //G
            ASGE::Colour(0.039f, 0.294f, 1.0f),         //B
            ASGE::Colour(0.552f, 0.039f, 1.0f),         //I
            ASGE::Colour(0.972f, 0.039f, 1.0f)          //V
    };
};

