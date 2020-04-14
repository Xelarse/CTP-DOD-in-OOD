#include "game.h"
int main()
{
  ASGE::GameSettings game_settings;
  game_settings.window_title  = "CTP DOD in OOD";
  game_settings.window_width  = 1600;
  game_settings.window_height = 900;
  game_settings.msaa_level    = 2;
  game_settings.fps_limit = 1000;

  MyASGEGame asge_game(game_settings);
  if (asge_game.init())
  {
    asge_game.run();
  }
  return 0;
}