#pragma once
#include <Engine/Camera2D.hpp>
#include <Engine/OGLGame.h>
#include <string>

#include "Scenes/BaseScene.h"

class MyASGEGame : public ASGE::OGLGame
{
 public:
  explicit MyASGEGame(ASGE::GameSettings settings);
  ~MyASGEGame() override;
  bool init();

  enum class Scenes
  {
      MENU,
      NO_SYSTEMS,
      SYSTEMS
  };

  void ChangeScene(Scenes sceneToSwitchTo);
  void TerminateProgram();

 private:
  void initCamera();
  void keyHandler(const ASGE::SharedEventData& data);
  void logStartup() const;
  void render() override;
  void update(const ASGE::GameTime&) override;

  int key_callback_id           = -1; /**< Key Input Callback ID. */

  // ASGEv2 examples
  ASGE::Camera2D camera{};

  std::unique_ptr<BaseScene> _menuScene;
  std::unique_ptr<BaseScene> _noSysScene;
  std::unique_ptr<BaseScene> _sysScene;

  BaseScene* _activeScene = nullptr;

  double _totalCount = 0;
};