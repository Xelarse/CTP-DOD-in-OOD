//
// Created by Alex on 4/4/2020.
//

#include "NoSystemsScene.h"
#include "game.h"

NoSystemsScene::NoSystemsScene(MyASGEGame *gameRef) : BaseScene(gameRef)
{

}

NoSystemsScene::~NoSystemsScene()
{

}

void NoSystemsScene::PreUpdate(double dt)
{

}

void NoSystemsScene::Update(double dt)
{

}

void NoSystemsScene::PostUpdate(double dt)
{

}

void NoSystemsScene::Render(ASGE::Renderer *renderer)
{

}

void NoSystemsScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_ESCAPE)
    {
        _gameRef->ChangeScene(MyASGEGame::Scenes::MENU, true);
    }
}
