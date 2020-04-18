//
// Created by Alex on 4/18/2020.
//

#include "ConfigurationScene.h"
#include "game.h"

ConfigurationScene::ConfigurationScene(MyASGEGame *gameRef, ConfigurationScene::SceneToLoad nextScene) : BaseScene(gameRef), _nextScene(nextScene)
{

}
void ConfigurationScene::PreUpdate(double dt)
{

}

void ConfigurationScene::Update(double dt)
{

}

void ConfigurationScene::PostUpdate(double dt)
{

}

void ConfigurationScene::Render(ASGE::Renderer *renderer)
{
	std::string text;

	text = _selectionCounter == 0 ? "! ~1K entities !" : "~1K entities";
	renderer->renderText(
			text,
			static_cast<int>(ASGE::SETTINGS.window_width * 0.45),
			static_cast<int >(ASGE::SETTINGS.window_height * 0.2),
			_selectionCounter == 0 ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
	);

	text = _selectionCounter == 1 ? "! ~10K entities !" : "~10K entities";
	renderer->renderText(
			text,
			static_cast<int>(ASGE::SETTINGS.window_width * 0.45),
			static_cast<int >(ASGE::SETTINGS.window_height * 0.4),
			_selectionCounter == 1 ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
	);

	text = _selectionCounter == 2 ? "! ~100K entities !" : "~100K entities";
	renderer->renderText(
			text,
			static_cast<int>(ASGE::SETTINGS.window_width * 0.45),
			static_cast<int >(ASGE::SETTINGS.window_height * 0.6),
			_selectionCounter == 2 ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
	);

	text = _selectionCounter == 3 ? "! ~1mil entities !" : "~1mil entities";
	renderer->renderText(
			text,
			static_cast<int>(ASGE::SETTINGS.window_width * 0.45),
			static_cast<int >(ASGE::SETTINGS.window_height * 0.8),
			_selectionCounter == 3 ? ASGE::COLOURS::DARKGRAY : ASGE::COLOURS::BLACK
	);

	std::string title = _nextScene == SceneToLoad::NO_SYSTEMS ? "No Systems:" : "Allman Systems:";
	renderer->renderText(
			"Entity Test count for " + title,
			static_cast<int>(ASGE::SETTINGS.window_width * 0.4),
			static_cast<int >(ASGE::SETTINGS.window_height * 0.025),
			ASGE::COLOURS::BLACK
	);
}

void ConfigurationScene::KeyHandler(const ASGE::SharedEventData &data)
{
	auto key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

	if(key->action != 1) { return; }

	if (key->key == ASGE::KEYS::KEY_DOWN || key->key == ASGE::KEYS::KEY_S)
	{
		_selectionCounter = _selectionCounter + 1 >= static_cast<int>(_demoCounts.size()) ?
		                    static_cast<int>(_demoCounts.size()) - 1 : _selectionCounter + 1;
	}
	else if (key->key == ASGE::KEYS::KEY_UP || key->key == ASGE::KEYS::KEY_W)
	{
		_selectionCounter = _selectionCounter - 1 < 0 ?
		                    0 : _selectionCounter - 1;
	}
	else if (key->key == ASGE::KEYS::KEY_ENTER || key->key == ASGE::KEYS::KEY_SPACE)
	{
		if(_gameRef != nullptr)
		{
			auto sceneToLoad = _nextScene == SceneToLoad::NO_SYSTEMS ?
					MyASGEGame::Scenes::NO_SYSTEMS :
					MyASGEGame::Scenes::SYSTEMS;

			_gameRef->ChangeScene(sceneToLoad, _demoCounts[static_cast<unsigned long long int>(_selectionCounter)]);
		}
	}
	else if (key->key == ASGE::KEYS::KEY_ESCAPE)
	{
		if(_gameRef != nullptr)
		{
			_gameRef->ChangeScene(MyASGEGame::Scenes::MENU);
		}
	}
}

