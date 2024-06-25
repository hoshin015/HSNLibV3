#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <map>
#include <string>
#include <variant>
using namespace DirectX;

#include "../Graphics/RenderContext.h"

// TODO : ������Manager������ĕ����Ǘ�����
class Camera
{
private:
	Camera();

	~Camera()
	{
	}

public:
	static Camera& Instance()
	{
		static Camera instance;
		return instance;
	}

	// ����
	void Input();

	// �X�V
	void Update();

	void FreeCameraUpdate();
	void TargetCameraUpdate();
	void ModelEditorCameraUpdate();
	void LockCameraUpdate();
	void ModelEidtorUpdate();

	// �^�[�Q�b�g�ʒu�ݒ�
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// view �ݒ�
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// projection �ݒ�
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

	// --- �Q�b�^�[ ---

	// View �擾
	const DirectX::XMFLOAT4X4& GetView() const { return view; }
	// Projection �擾
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// fov �擾
	const float& GetFovY() const { return fovY; }

	// aspect �擾
	const float& GetAspect() const { return aspect; }

	// nearZ �擾
	const float& GetNearZ() const { return nearZ; }
	// farZ �擾
	const float& GetFarZ() const { return farZ; }

	// ���_�擾
	const DirectX::XMFLOAT3& GetEye() const { return eye; }
	// �����_�擾
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	// ������擾
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	// �O�����擾
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	// �E�����擾
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	// angle �ݒ�
	void SetAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; }

	// �萔�o�b�t�@�̍X�V
	void UpdateCameraConstant();

public:
	enum CAMERA
	{
		TARGET_PLAYER,
		FREE,
		MODEL_EDITOR,
		LOCK,
		ModelEditor,
	};

	int cameraType;

private:
	// View Projection �s��
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	// fov �J�����̎���p
	float fovY = 30;

	// aspect ��ʂ̏c����	
	float aspect;

	// far near �N���b�v����
	float nearZ = 1.0f;
	float farZ  = 1000.0f;

	// �J�����̈ʒu
	DirectX::XMFLOAT3 eye;
	// �J�����̒����_
	DirectX::XMFLOAT3 focus;
	// �J�����̃^�[�Q�b�g�ʒu
	DirectX::XMFLOAT3 target;

	// �J�����̕����x�N�g��
	DirectX::XMFLOAT3 up;    // ������x�N�g��
	DirectX::XMFLOAT3 front; // �O�����x�N�g��
	DirectX::XMFLOAT3 right; // �E�����x�N�g��

	// �J�����̉�]
	DirectX::XMFLOAT3 angle;


	float maxAngleX   = DirectX::XMConvertToRadians(30);
	float minAngleX   = DirectX::XMConvertToRadians(-5);
	float limitAngleX = DirectX::XMConvertToRadians(89);

	// �J�[�\�����W
	float oldCursorX = 0.0f;
	float oldCursorY = 0.0f;
	float difX       = 0.0f;
	float difY       = 0.0f;

	// ���̓f�[�^�ۊǗp
	using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, inputData> inputMap;

	template <typename T>
	T GetInput(std::string key)
	{
		return std::get<T>(inputMap[key]);
	}
};
