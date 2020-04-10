//
// Created by Alex on 4/4/2020.
//
#include "BaseScene.h"

class SystemsScene : public BaseScene
{
public:
    SystemsScene() = delete;
    explicit SystemsScene(MyASGEGame* gameRef, ASGE::Renderer* renderer);
    ~SystemsScene() override;

    void PreUpdate(double dt) override;
    void Update(double dt) override;
    void PostUpdate(double dt) override;
    void Render(ASGE::Renderer *renderer) override;
    void KeyHandler(const ASGE::SharedEventData &data) override;
};
