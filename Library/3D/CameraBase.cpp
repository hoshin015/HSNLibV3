#include "CameraBase.h"

#include "../RegisterNum.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/RenderContext.h"


CameraBase::CameraBase()
{
	//Graphics* gfx = &Graphics::Instance();

	//std::lock_guard<std::mutex> lock(gfx->GetMutex()); // �r������
	//ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// --- �v���W�F�N�V�������W�ϊ��s��쐬 ---
	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	//dc->RSGetViewports(&numViewports, &viewport);

	aspect = 1.7777777778f;

	Update();
}


// --- �J�����s��̍X�V ---
void CameraBase::Update()
{
	Update(position, target, up, fov, nearZ, farZ, aspect);
}


void CameraBase::Update(const Vector3& position, const Vector3& target, const Vector3& up, const float fov, const float nearZ, const float farZ, const float aspect)
{	
	// --- �s��̌v�Z ---
	view.MakeLookAt(position, target, up);
	projection.MakePerspective(fov, aspect, nearZ, farZ);

	// --- �r���[�s����t�s�񉻂��āA���[���h�s����v�Z���� ---
	Matrix world = Matrix::Inverse(view);

	// --- �s�񂩂�����x�N�g�������o�� ---
	frontVec = Vector3::Normalize(world.v_[2].xyz());
	upVec = Vector3::Normalize(world.v_[1].xyz());
	rightVec = Vector3::Normalize(world.v_[0].xyz());

}


// --- �萔�o�b�t�@�̍X�V ---
void CameraBase::UpdateConstants()
{
	UpdateConstants(position);
}


void CameraBase::UpdateConstants(const Vector3& position)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	RenderContext* rc = &RenderContext::Instance();

	// --- ���̌v�Z�Ɛݒ� ---
	Matrix viewProjection = view * projection;
	Matrix InverseViewProjection = Matrix::Inverse(viewProjection);
	rc->cameraConstant.viewProjection = viewProjection.mat_;
	rc->cameraConstant.inverseViewProjection = InverseViewProjection.mat_;
	rc->cameraConstant.cameraPosition = { position.x, position.y, position.z, 1.0f/*0*/ };

	// --- �萔�o�b�t�@�̍X�V�ƃo�C���h ---
	dc->UpdateSubresource(rc->cameraConstantBuffer.Get(), 0, 0, &rc->cameraConstant, 0, 0);
	dc->VSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	//dc->CSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());

}
