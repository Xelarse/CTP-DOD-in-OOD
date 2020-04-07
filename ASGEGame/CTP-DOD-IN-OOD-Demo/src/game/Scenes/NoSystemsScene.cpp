//
// Created by Alex on 4/4/2020.
//

#include "NoSystemsScene.h"
#include "game.h"

NoSystemsScene::NoSystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer) : BaseScene(gameRef)
{
    _testSquare = std::make_unique<Square>(renderer);
}

NoSystemsScene::~NoSystemsScene()
{

}

void NoSystemsScene::PreUpdate(double dt)
{

}

void NoSystemsScene::Update(double dt)
{
    _testSquare->Update(dt);
}

void NoSystemsScene::PostUpdate(double dt)
{

}

void NoSystemsScene::Render(ASGE::Renderer *renderer)
{
    std::string text = "NO SYSTEMS SCENE ACTIVE";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.5),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.5),
            ASGE::COLOURS::BLACK
    );


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
