//
// Created by Alex on 4/6/2020.
//

#include "MenuScene.h"
#include "game.h"

MenuScene::MenuScene(MyASGEGame *gameRef) : BaseScene(gameRef)
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
    std::string text;
    const auto curSel = GetCurrentSelection();

    text = curSel == Selection::SYSTEMS ? "! Systems !" : "Systems";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.5),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.6),
            curSel == Selection::SYSTEMS ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
    );

    text = curSel == Selection::NO_SYSTEMS ? "! No Systems !" : "No Systems";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.5),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.3),
            curSel == Selection::NO_SYSTEMS ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
    );

    text = curSel == Selection::QUIT ? "! Quit !" : "Quit";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.5),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.9),
            curSel == Selection::QUIT ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
    );
}

void MenuScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if(key->action != 1) { return; }

    if (key->key == ASGE::KEYS::KEY_DOWN || key->key == ASGE::KEYS::KEY_S)
    {
        _currentSelection = _currentSelection + 1 >= static_cast<int>(Selection::COUNT) ?
                static_cast<int>(Selection::COUNT) - 1 : _currentSelection + 1;
    }
    else if (key->key == ASGE::KEYS::KEY_UP || key->key == ASGE::KEYS::KEY_W)
    {
        _currentSelection = _currentSelection - 1 < 0 ?
                0 : _currentSelection - 1;
    }
    else if (key->key == ASGE::KEYS::KEY_ENTER || key->key == ASGE::KEYS::KEY_SPACE)
    {
        if(_gameRef != nullptr)
        {
            Selection option = GetCurrentSelection();
            switch(option)
            {
                case Selection::NO_SYSTEMS:
                    _gameRef->ChangeScene(MyASGEGame::Scenes::NO_SYSTEMS);
                    break;
                case Selection::SYSTEMS:
                    _gameRef->ChangeScene(MyASGEGame::Scenes::SYSTEMS);
                    break;
                case Selection::QUIT:
                    _gameRef->TerminateProgram();
                    break;
                default:
                    break;
            }
        }
    }
}

MenuScene::Selection MenuScene::GetCurrentSelection()
{
    int test = _currentSelection % static_cast<int>(Selection::COUNT);
    return static_cast<Selection>(test);
}
