#pragma once
#include <string>

//--------------------------------------------------------------
//  Scene
//--------------------------------------------------------------
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}


	// ������
	virtual void Initialize() = 0;

	// �I����
	virtual void Finalize() = 0;

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Render() = 0;

	// �����������Ă��邩
	bool IsReady() const { return ready; }
	// ���������ݒ�
	void SetReady() { ready = true; }

protected:
	// ���j���[�o�[�̕`��
	void DrawMenuBar();
	// �^�C�}�[�萔�o�b�t�@�̍X�V
	void UpdateTimerConstnat();

private:
	bool ready = false;
};