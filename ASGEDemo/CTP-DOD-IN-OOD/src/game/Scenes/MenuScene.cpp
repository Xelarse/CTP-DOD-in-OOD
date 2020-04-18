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

    text = curSel == Selection::SYSTEMS ? "! Allman Systems !" : "Allman Systems";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.43),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.5),
            curSel == Selection::SYSTEMS ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
    );

    text = curSel == Selection::NO_SYSTEMS ? "! No Systems !" : "No Systems";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.44),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.4),
            curSel == Selection::NO_SYSTEMS ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
    );

    text = curSel == Selection::QUIT ? "! Quit !" : "Quit";
    renderer->renderText(
            text,
            static_cast<int>(ASGE::SETTINGS.window_width * 0.46),
            static_cast<int >(ASGE::SETTINGS.window_height * 0.6),
            curSel == Selection::QUIT ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
    );

    renderer->renderText(
    		"-Up and Down arrow keys to navigate menus\n\n\n-Enter to select an option\n\n\n-Escape to leave a demo scene and return to menu",
		    static_cast<int>(ASGE::SETTINGS.window_width * 0.01),
    		static_cast<int>(ASGE::SETTINGS.window_height * 0.85),
    		ASGE::COLOURS::BLACK
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
                    _gameRef->ChangeScene(MyASGEGame::Scenes::CONFIG, 0, MyASGEGame::Scenes::NO_SYSTEMS);
                    break;
                case Selection::SYSTEMS:
                    _gameRef->ChangeScene(MyASGEGame::Scenes::CONFIG, 0, MyASGEGame::Scenes::SYSTEMS);
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
