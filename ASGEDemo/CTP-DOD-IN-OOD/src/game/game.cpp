#include <memory>
#include <string>

#include "game.h"
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Logger.hpp>
#include <Engine/Point2D.h>
#include <Engine/Sprite.h>

#include "Scenes/MenuScene.h"
#include "Scenes/NoSystemsScene.h"
#include "Scenes/SystemsScene.h"
#include "Timer.h"


MyASGEGame::MyASGEGame(ASGE::GameSettings settings) : OGLGame(settings)
{
  renderer->setClearColour(ASGE::COLOURS::DIMGREY);
  inputs->use_threads = false;
  toggleFPS();
  initCamera();
//  logStartup();
}

void MyASGEGame::initCamera()
{
  // create a camera pointing
  camera = ASGE::Camera2D{ static_cast<float>(ASGE::SETTINGS.window_width),
                           static_cast<float>(ASGE::SETTINGS.window_height) };

  // point the camera towards the middle of the window
  camera.lookAt(
    ASGE::Point2D{ static_cast<float>(-ASGE::SETTINGS.window_width) / 2.f,
                   static_cast<float>(-ASGE::SETTINGS.window_height) / 2.f });
}

void MyASGEGame::logStartup() const
{
  // these are just examples of the logging system.. you dont need to keep em
  std::ostringstream ss{ "initialisation complete" };
  Logging::ERRORS(ss.str());
  Logging::WARN(ss.str());
  Logging::INFO(ss.str());
  Logging::DEBUG(ss.str());
  Logging::TRACE(ss.str());
  Logging::log(
    Logging::timestamp() + " \x1b[35;1m[CUSTOM]\x1b[0m " + ss.str() +
    '\n');
}

MyASGEGame::~MyASGEGame()
{
  this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));
}

bool MyASGEGame::init()
{
  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &MyASGEGame::keyHandler, this);

  _menuScene = std::make_unique<MenuScene>(this);
  _noSysScene = std::make_unique<NoSystemsScene>(this, renderer.get());
  _sysScene = std::make_unique<SystemsScene>(this, renderer.get());

  _activeScene = _menuScene.get();
  return true;
}

void MyASGEGame::keyHandler(const ASGE::SharedEventData& data)
{
    if (_activeScene != nullptr)
    {
        _activeScene->KeyHandler(data);
    }
}

void MyASGEGame::update(const ASGE::GameTime& game_time)
{
   auto dt = game_time.delta.count();
   _totalCount = static_cast<double>(game_time.elapsed.count());

  if(_activeScene != nullptr)
  {
      _activeScene->UpdateTotalTime(_totalCount);
	  {
//	  	Timer preTimer([](long long dura){Logging::INFO("PreUpdate: " + std::to_string(dura)); });
        _activeScene->PreUpdate(dt);
	  }
	  {
//		  Timer updateTimer([](long long dura){Logging::INFO("Update: " + std::to_string(dura)); });
	  	_activeScene->Update(dt);
      }
	  {
//		  Timer postTimer([](long long dura){Logging::INFO("PostUpdate: " + std::to_string(dura)); });
	  	_activeScene->PostUpdate(dt);
	  }

  }
}

void MyASGEGame::render()
{
  renderer->setFont(0);
  renderer->setProjectionMatrix(camera.getView());

  if (_activeScene != nullptr)
  {
      _activeScene->Render(renderer.get());
  }
}

void MyASGEGame::ChangeScene(MyASGEGame::Scenes sceneToSwitchTo, bool sceneReset)
{
    switch(sceneToSwitchTo)
    {
        case Scenes::MENU:
            if(sceneReset) {_menuScene = std::make_unique<MenuScene>(this);}
            _activeScene = _menuScene.get();
            break;
        case Scenes::NO_SYSTEMS:
            if(sceneReset) {_noSysScene = std::make_unique<NoSystemsScene>(this, renderer.get());}
            _activeScene = _noSysScene.get();
            break;
        case Scenes::SYSTEMS:
            if(sceneReset) {_sysScene = std::make_unique<SystemsScene>(this, renderer.get());}
            _activeScene = _sysScene.get();
            break;
    }
}

void MyASGEGame::TerminateProgram()
{
    signalExit();
}
