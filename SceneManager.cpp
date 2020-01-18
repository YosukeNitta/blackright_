#include <DxLib.h>
#include <memory>
#include "SceneManager.h"

//	�f�X�g���N�^
SceneManager::~SceneManager()
{
	for (auto scene : _scenes) {
		delete scene.second;
	}
}

//	�N���X�̃C���X�^���X���擾
SceneManager & SceneManager::Instance()
{
	static std::unique_ptr<SceneManager> instance(new SceneManager);

	return *instance;
}

// �V�[����ǉ�
void SceneManager::AddScene(std::string sceneName, Scene* scene)
{
	if (_scenes.count(sceneName))
		return;
	_scenes[sceneName] = scene;
}

//	�J�n�V�[����ݒ�
void SceneManager::StartScene(std::string sceneName)
{
	if (!_scenes.count(sceneName))
		return;

	_currentScene = _scenes[sceneName];
}


//	�V�[���J��
void SceneManager::LoadScene(std::string sceneName)
{
	if (!_scenes.count(sceneName))
		return;

	_nextScene = _scenes[sceneName];
	_state = SceneState::Release;
}

//	�X�V����
void SceneManager::Update()
{
	//	���C�����[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		//	�w�i���N���A����
		ClearDrawScreen();

		//	�V�[�����X�V����
		SceneUpdate();

		//	�X�V
		ScreenFlip();
	}
}

//	�V�[���̍X�V����
void SceneManager::SceneUpdate()
{
	if (!_currentScene)
		return;
	//	������
	if (_state == SceneState::Initialize) {
		_currentScene->Initialize();
		_state = SceneState::Update;
	}

	//	�X�V����
	_currentScene->Update();

	//	�������
	if (_state == SceneState::Release) {
		_currentScene->Release();
		NextScene();
		_state = SceneState::Initialize;
	}
}

//	���̃V�[���ɐ؂�ւ���
void SceneManager::NextScene()
{
	_currentScene = _nextScene;
}