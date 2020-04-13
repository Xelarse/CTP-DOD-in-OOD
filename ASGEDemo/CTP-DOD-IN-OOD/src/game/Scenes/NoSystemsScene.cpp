//
// Created by Alex on 4/4/2020.
//

#include "NoSystemsScene.h"
#include "game.h"

NoSystemsScene::NoSystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer) : BaseScene(gameRef)
{
    Vector testPos(static_cast<float>(ASGE::SETTINGS.window_width) * 0.5f, static_cast<float>(ASGE::SETTINGS.window_height) * 0.5f);
    _testSquare = std::make_unique<Square>(renderer, testPos);
}

NoSystemsScene::~NoSystemsScene()
{

}

void NoSystemsScene::PreUpdate(double dt)
{

}

void NoSystemsScene::Update(double dt)
{
    _testSquare->UpdateSpritePosition(dt);
    _testSquare->UpdateSpriteScale(dt);
    _testSquare->UpdateSpriteColour(_currentTotalTime);
}

void NoSystemsScene::PostUpdate(double dt)
{

}

void NoSystemsScene::Render(ASGE::Renderer *renderer)
{
    _testSquare->Render(renderer);
}

void NoSystemsScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_ESCAPE)
    {
        _gameRef->ChangeScene(MyASGEGame::Scenes::MENU);
    }
}
