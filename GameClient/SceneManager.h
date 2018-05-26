#pragma once
#include "Scene.h"

class SceneManager {
public:
	Scene* scene;
	static SceneManager& Instance() {
		static SceneManager s;
		return s;
	}
	void Update(float delta);
public:
	SceneManager() {}
	SceneManager(SceneManager const&) = delete;
	void operator=(SceneManager const&) = delete;
};