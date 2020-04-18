//
// Created by Alex on 4/18/2020.
//

#include "BaseScene.h"

class ConfigurationScene : public BaseScene
{
public:

	enum class SceneToLoad
	{
		NO_SYSTEMS,
		ALLMAN_SYSTEMS
	};

	ConfigurationScene() = delete;
	explicit ConfigurationScene(MyASGEGame *gameRef, SceneToLoad nextScene);
	~ConfigurationScene() override = default;

	void PreUpdate(double dt) override;
	void Update(double dt) override;
	void PostUpdate(double dt) override;
	void Render(ASGE::Renderer *renderer) override;
	void KeyHandler(const ASGE::SharedEventData &data) override;

private:
	int _selectionCounter = 0;
	////500 ~ 1mil entities, 140 ~ 100k entities, 32 ~ 10k entities, 0 ~ 1.4k entities
	std::array<int, 4> _demoCounts = {0, 32, 140, 500};
	const SceneToLoad _nextScene;
};

