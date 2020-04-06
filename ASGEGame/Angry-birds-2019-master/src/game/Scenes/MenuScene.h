//
// Created by Alex on 4/6/2020.
//

#include "BaseScene.h"

class MenuScene : public BaseScene
{
public:
    MenuScene();
    ~MenuScene() override;

    void PreUpdate(float dt) override;

    void Update(float dt) override;

    void PostUpdate(float dt) override;

    void Render(ASGE::Renderer *renderer) override;

    void KeyHandler(const ASGE::SharedEventData &data) override;
};


