//
// Created by Alex on 4/4/2020.
//
#include "BaseScene.h"

class SystemsScene : public BaseScene
{
public:
    SystemsScene();
    ~SystemsScene() override;

    void PreUpdate(float dt) override;

    void Update(float dt) override;

    void PostUpdate(float dt) override;

    void Render(ASGE::Renderer *renderer) override;
};
