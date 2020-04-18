//
// Created by Alex on 4/4/2020.
//

#include "NoSystemsScene.h"
#include "game.h"
#include <Engine/Logger.hpp>

NoSystemsScene::NoSystemsScene(MyASGEGame *gameRef, ASGE::Renderer* renderer) : BaseScene(gameRef)
{
    double offset = Square::_posLimit + _squarePadding;
    int xCount = static_cast<int>(ASGE::SETTINGS.window_width / offset);
    int yCount = static_cast<int>(ASGE::SETTINGS.window_height / offset);

	_squares.reserve(static_cast<unsigned long long int>((xCount + 2 * _demoSpanMod) * (yCount + 2 * _demoSpanMod)));

    for(int x = -_demoSpanMod; x < xCount + _demoSpanMod; ++x)
    {
        for(int y = -_demoSpanMod; y < yCount + _demoSpanMod; ++y)
        {
            _squares.emplace_back(std::make_unique<Square>(renderer, Vector(static_cast<float>(x * offset), static_cast<float>(y * offset))) );
        }
    }

	std::ostringstream ss{ "No System scene square count :" + std::to_string(_squares.size()) };
	Logging::INFO(ss.str());
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
	    //Sorry in advance for this james, Checking in they're in window view for rendering
	    //Yeah might change this to use camera bounds if i mess with the camera view
	    Vector pos = square->GetPosition();
	    if(pos._x >= 0 && pos._x <= static_cast<float>(ASGE::SETTINGS.window_width) &&
	       pos._y >= 0 && pos._y <= static_cast<float>(ASGE::SETTINGS.window_height) )
	    {
		    square->Render(renderer);
	    }
    }

	renderer->renderText(
			"Entites updated per tick: " + std::to_string(_squares.size()) + "\nAll entities are updated per tick but\nOnly entities in screen view are rendered",
			1120,
			30,
			ASGE::COLOURS::BLACK
	);
}

void NoSystemsScene::KeyHandler(const ASGE::SharedEventData &data)
{
    auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_ESCAPE)
    {
        _gameRef->ChangeScene(MyASGEGame::Scenes::MENU);
    }
}
