//
// Created by Alex on 4/4/2020.
//

#include "NoSystemsScene.h"
#include "game.h"

NoSystemsScene::NoSystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer) : BaseScene(gameRef)
{
    double offset = Square::_posLimit + _squarePadding;
    int xCount = static_cast<int>(ASGE::SETTINGS.window_width / offset);
    int yCount = static_cast<int>(ASGE::SETTINGS.window_height / offset);

    for(int x = -5; x < xCount + 5; ++x)
    {
        for(int y = -5; y < yCount + 5; ++y)
        {
            _squares.emplace_back(std::make_unique<Square>(renderer, Vector(static_cast<float>(x * offset), static_cast<float>(y * offset))) );
        }
    }
}

NoSystemsScene::~NoSystemsScene()
{

}

void NoSystemsScene::PreUpdate(double dt)
{

}

void NoSystemsScene::Update(double dt)
{
    for(auto& square : _squares)
    {
        square->UpdateSpritePosition(dt);
        square->UpdateSpriteScale(dt);
        square->UpdateSpriteColour(_currentTotalTime);
    }
}

void NoSystemsScene::PostUpdate(double dt)
{

}

void NoSystemsScene::Render(ASGE::Renderer *renderer)
{
    for(auto& square : _squares)
    {
        square->Render(renderer);
    }
}

void NoSystemsScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_ESCAPE)
    {
        _gameRef->ChangeScene(MyASGEGame::Scenes::MENU);
    }
}
