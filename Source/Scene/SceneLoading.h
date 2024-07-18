#pragma once
#include <thread>
#include "Scene.h"
#include "../../Library/2D/Sprite.h"

// �^�C�g���V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene){}
	~SceneLoading() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

private:
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;

	std::unique_ptr<Sprite> bg;
	std::unique_ptr<Sprite> sprite;

	float loadTime = 5.0f;
	float loadTimer = 0.0f;
};
