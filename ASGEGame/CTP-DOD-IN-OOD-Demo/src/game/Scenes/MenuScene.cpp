//
// Created by Alex on 4/6/2020.
//

#include "MenuScene.h"
#include "game.h"

MenuScene::MenuScene(MyASGEGame *gameRef) : BaseScene(gameRef)
{

}

MenuScene::~MenuScene()
{

}
void MenuScene::PreUpdate(double dt)
{

}

void MenuScene::Update(double dt)
{

}

void MenuScene::PostUpdate(double dt)
{

}

void MenuScene::Render(ASGE::Renderer *renderer)
{

}

void MenuScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_UP || key->key == ASGE::KEYS::KEY_W)
    {
        ++_currentSelection;
    }
    else if (key->key == ASGE::KEYS::KEY_DOWN || key->key == ASGE::KEYS::KEY_S)
    {
        --_currentSelection;
    }
    else if (key->key == ASGE::KEYS::KEY_ENTER || key->key == ASGE::KEYS::KEY_SPACE)
    {
        if(_gameRef != nullptr)
        {
            Selection option = static_cast<Selection>(_currentSelection);
            switch(option)
            {
                case Selection::NO_SYSTEMS:
                    _gameRef->ChangeScene(MyASGEGame::Scenes::NO_SYSTEMS, true);
                    break;
                case Selection::SYSTEMS:
                    _gameRef->ChangeScene(MyASGEGame::Scenes::SYSTEMS, true);
                    break;
                case Selection::QUIT:
                    _gameRef->TerminateProgram();
                    break;
            }
        }
    }
}
